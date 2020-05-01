#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif
#include "tsk_control.h"
#include "srv_psensor.h"
#include "srv_sov.h"
#include "srv_pot.h"

// \todo JB review max durations for each of these 
#define DURATION_100CC	50
#define INHALE_12BPM	1500
#define EXHALE_12BPM	3000
static systemstate_t IDLE_ST = {IDLE, 0xFFFFFFFF};
static systemstate_t FILL_TANK_ST = {FILL_TANK, 200}; // 200ms will fill to 6psi at 60psi input pressure to give 400cc tidal volume
static systemstate_t FILL_ADJUST_ST = {FILL_ADJUST, 300}; // 300ms additionnal to reach 15psi to give 1000cc tidal volume
static systemstate_t INHALE_ST = {INHALE, INHALE_12BPM}; // default is 12 bpm - 1.5s inhale / 3.5 exhale (500ms included in fill states)
static systemstate_t EXHALE_ST = {EXHALE, EXHALE_12BPM}; 


static systemstate_t *VENT_STATES[] = {&IDLE_ST, &FILL_TANK_ST, &FILL_ADJUST_ST, &INHALE_ST, &EXHALE_ST, NULL};
const uint8_t FIRST_STATE_IDX = 1;

// \todo JB update based on final architecture
static psensor_data_t tanksensor = { 0,0,0,0,0,SENSOR_M3200, porti2c1, 0x28, "Tank"};
static psensor_data_t patientsensor = { 0,0,0,0,0,SENSOR_HSC, portspi, CS_SPI_PSIG4, "Patient"};
static psensor_data_t systeminputsensor = { 0,0,0,0,0,SENSOR_M3200, porti2c0, 0x28, "Input"};

static psensor_data_t *sensors[] = {&systeminputsensor, &tanksensor, &patientsensor, NULL};


static potentiometer_t bpm_input = {12, 20, BPM_POT, 0, 0}; 
static potentiometer_t tidal_input = {0, 6, TIDAL_POT, 0, 0}; 

static potentiometer_t *potentiometers[] = {&bpm_input, &tidal_input, NULL};


static const uint32_t task_interval_ms = TSK_CONTROL_PERIOD;
static uint32_t task_prev = 0; 

static uint32_t curr_duration_in_state = FIRST_STATE_IDX;
static uint8_t curr_state = 0;

static float CFG_TANK_PRESSURE = 6.0;

// 50 cmH2O - safety
const float MAX_PATIENT_PRESSURE = 0.7112;


void adjustBPM()
{
	if((bpm_input.min <= bpm_input.value) && (bpm_input.max >= bpm_input.value))
	{
		INHALE_ST.max_duration = INHALE_12BPM - ((bpm_input.value-bpm_input.min)*0.5 / 8)*1000; // 12 bpm - 1.5s // 20 bpm 1s
		EXHALE_ST.max_duration = EXHALE_12BPM - ((bpm_input.value-bpm_input.min)*1.5 / 8)*1000;  // 12 bpm - 3.5s // 20 bpm 2s
	}
}

void adjustTidalVolume()
{
	if((tidal_input.min <= tidal_input.value) && (tidal_input.max >= tidal_input.value))
	{
		FILL_ADJUST_ST.max_duration = DURATION_100CC * tidal_input.value;
    CFG_TANK_PRESSURE = 6.0 + tidal_input.value * 1.5;
	}
}


void change_state()
{
	// \todo JB get rid of switch and implement action vectors for each state
	curr_duration_in_state = 0;
	curr_state++;
	if(VENT_STATES[curr_state] == NULL)
		curr_state = FIRST_STATE_IDX;
	
	switch(VENT_STATES[curr_state]->state)
	{
		case IDLE:
			sov_close(TANK_SOV);
			sov_close(OUT_SOV);  
			sov_open(PATIENT_SOV);
			break;
		case FILL_TANK:
			adjustBPM();
			adjustTidalVolume();
			sov_close(PATIENT_SOV);
			sov_open(OUT_SOV); 	
			sov_open(TANK_SOV);
			break;
		case FILL_ADJUST:
			sov_close(PATIENT_SOV);
			sov_open(OUT_SOV); 	
			sov_open(TANK_SOV);
			break;
		case INHALE:
			sov_close(TANK_SOV);
			sov_close(OUT_SOV); 
			sov_open(PATIENT_SOV);	
			break;
		case EXHALE:
			sov_close(TANK_SOV);
			sov_close(PATIENT_SOV);	
			sov_open(OUT_SOV);  
			break;
		default:
			sov_close(TANK_SOV);
			sov_close(PATIENT_SOV);
			sov_open(OUT_SOV); 
			
	}
}


void tsk_control_init()
{
  // JB \todo change to array - must change function
  ps_init(patientsensor);
  sov_init();
  pot_init();
  curr_state = FIRST_STATE_IDX;
  curr_duration_in_state = 0;
}

void tsk_control()
{
	uint8_t el;
	uint32_t now = millis();
	uint32_t elapsed = now - task_prev;

	// \todo JB CHECK FOR SERIAL COMMS
//    if ((elapsed > task_interval_ms) && (Serial.available() <= 0))
	if (elapsed > task_interval_ms)	
	{
        task_prev = now;
        
		// log all sensors all the time
		Serial.print(now);
		for(uint8_t i=0 ; sensors[i] != NULL ; i++)
		{
			// \todo JB HANDLE ERRORS (el) + COMM TIMEOUTS
			// \todo JB change architecture to make sure the control loop keeps the 5ms no matter the update sensor failures?
			// \todo JB only acquire sensor relevant to current state?
			el = ps_update_sensor(sensors[i]);
			Serial.print(",");
			Serial.print(sensors[i]->pressure,4);
		}
		Serial.print(",");
		Serial.print(VENT_STATES[curr_state]->state);
	
		// update inputs all the time
		for(uint8_t i=0 ; potentiometers[i] != NULL ; i++)
		{
			pot_update_input(potentiometers[i]);
			Serial.print(",");
			Serial.print(potentiometers[i]->value);
		}

		Serial.print('\n');

		curr_duration_in_state += elapsed;
		
		if(curr_duration_in_state >= VENT_STATES[curr_state]->max_duration)
		{
			change_state();
		}
		else
		{
			// \todo JB check for early change of state (eg pressure reached)
			switch(VENT_STATES[curr_state]->state)
			{
				case FILL_TANK:
					if(tanksensor.pressure >= CFG_TANK_PRESSURE)
						change_state();
					break;
				case FILL_ADJUST:
					if(tanksensor.pressure >= CFG_TANK_PRESSURE)
						sov_close(TANK_SOV);
					break;
				case INHALE:
					// \todo JB alarm
					if(patientsensor.pressure >= MAX_PATIENT_PRESSURE)
						change_state();
					break;
				default:
					break;
			}
		}
	}
}
