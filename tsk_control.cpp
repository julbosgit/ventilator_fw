#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif
#include "tsk_control.h"
#include "srv_psensor.h"
#include "srv_sov.h"
#include "srv_pot.h"
#include "srv_screen.h"

#define MIN_INPUT_PRESSURE 20

// \todo JB review max durations for each of these 
#define MIN_TIDAL		400
#define DURATION_100CC	50 // chris: why not measure this in a diagnostic. Can we store constants?
#define INHALE_12BPM	1500
#define EXHALE_12BPM	3000
static systemstate_t IDLE_ST = {IDLE, 0xFFFFFFFF};
//Chris: we should have a line for tank size that can be entered in the code easily. The tank is now 1.2L so I am changing the code below. 
//Chris: I'm not sure where you got fill times from I guess Dave did that with restrictor and verified. Can we have that as a startup diagnostic? 
//Chris: Fill 200 ms with P1 measured see pressure in tank and use that for future ref.
static systemstate_t FILL_TANK_ST = {FILL_TANK, 200}; // 200ms will fill to (6 for 1L tank) 5psi at 60psi input pressure to give 400cc tidal volume
static systemstate_t FILL_ADJUST_ST = {FILL_ADJUST, 300}; // 300ms additionnal to reach  (14.7 for 1L tank) 12.25 psi to give 1000cc tidal volume

static systemstate_t INHALE_ST = {INHALE, INHALE_12BPM}; // default is 12 bpm - 1.5s inhale / 3.5 exhale (500ms included in fill states)
static systemstate_t EXHALE_ST = {EXHALE, EXHALE_12BPM}; 
static systemstate_t *VENT_STATES[] = {&IDLE_ST, &FILL_TANK_ST, &FILL_ADJUST_ST, &INHALE_ST, &EXHALE_ST, NULL};
const uint8_t FIRST_STATE_IDX = 1;

// \todo JB update based on final architecture
static psensor_data_t tanksensor = { 0,0,0,0,0,SENSOR_M3200, porti2c1, 0x28, "Tank"}; //reservoir to deliver gas to patient
static psensor_data_t patientsensor = { 0,0,0,0,0,SENSOR_HSC, portspi, CS_SPI_PSIG4, "Patient"}; // pressure sensor measurement at the patient
static psensor_data_t systeminputsensor = { 0,0,0,0,0,SENSOR_M3200, porti2c0, 0x28, "Input"}; // compressed gas inlet pressure
static psensor_data_t patientflowmeter1 = { 0,0,0,0,0,SENSOR_SSC, portspi, CS_SPI_FM1, "flowmeter"}; //dm 26may added flowmeter1 // differential pressure venturi measurement at patient
static psensor_data_t patientflowmeter2 = { 0,0,0,0,0,SENSOR_HSC, portspi, CS_SPI_FM2, "flowmeter"}; //dm 26may added flowmeter1 // differential pressure venturi measurement exhale

static psensor_data_t *sensors[] = {&systeminputsensor, &tanksensor, &patientsensor, &patientflowmeter1, &patientflowmeter2, NULL}; //dm 26may added flowmeter1 and 2

static potentiometer_t tidal_input = {0, 6, TIDAL_POT, 0, 0}; //Pot on enclosure far left
static potentiometer_t bpm_input = {12, 20, BPM_POT, 0, 0}; // 2nd from left 
static potentiometer_t low_airway_input = {0, 20, LOW_AIRWAY_ALERT_POT, 0, 0}; //dm 26may - 3rd from left
static potentiometer_t high_airway_input = {0, 60, HIGH_AIRWAY_ALERT_POT, 0, 0}; //dm 26may -4th from left
static potentiometer_t o2_input = {0, 100, O2_PERCENT_POT, 0, 0}; //dm 26may added  - 5th from left

static potentiometer_t *potentiometers[] = {&bpm_input, &tidal_input,&low_airway_input, &high_airway_input, &o2_input, NULL}; //dm 26may added pots

static uint16_t cfg_tidal_val;
static uint8_t cfg_bpm_val;

static char cfg_ie_ratio_val[] = "1/3\0";

static const uint32_t task_interval_ms = TSK_CONTROL_PERIOD;
static uint32_t task_prev = 0; 

static uint32_t curr_duration_in_state = FIRST_STATE_IDX;
static uint8_t curr_state = 0;

static float CFG_TANK_PRESSURE = 5.0; //chris: does this need to be changed to 5 to account for 1.2L tank or is it the scale 

// \todo switch to input - 50 cmH2O - safety
static uint8_t MAX_PATIENT_PRESSURE_CFG = 50; // Chris: should be set with potentiometer_t high_airway_input. The other relief is mechanically set.
static float MAX_PATIENT_PRESSURE = MAX_PATIENT_PRESSURE_CFG/70.307; //convert to psi

static uint8_t alarmalarm;

//Flag for volume
int volume_flag=0;


// ***** SCREEN PARAMETERS *******
screen_param_t cfg_max_peak = {(uint16_t*)&MAX_PATIENT_PRESSURE_CFG,integer,0,0,2, integer}; // position one on the screen. Chris: this value is set with pot highway air pressure but is not actual PP 
screen_param_t cfg_tidal = {(uint16_t*)&cfg_tidal_val,integer,0,3,4, integer};
screen_param_t cfg_bpm = {(uint16_t*)&cfg_bpm_val,integer,0,8,2, integer};
screen_param_t cfg_ie_ratio = {&cfg_ie_ratio_val,string,0,14,3,string};
screen_param_t alarm_screen = {&alarmalarm,integer,3,1,1,integer};

screen_param_t *screen_params[] = {&cfg_max_peak, &cfg_tidal, &cfg_bpm, &cfg_ie_ratio,&alarm_screen, NULL };

void adjustBPM()
{
	if((bpm_input.min <= bpm_input.value) && (bpm_input.max >= bpm_input.value))
	{
		INHALE_ST.max_duration = INHALE_12BPM - ((bpm_input.value-bpm_input.min)*0.5 / 8)*1000; // 12 bpm - 1.5s // 20 bpm 1s //chris: this implies a constant I/E Ratio for now ok
		EXHALE_ST.max_duration = EXHALE_12BPM - ((bpm_input.value-bpm_input.min)*1.5 / 8)*1000;  // 12 bpm - 3.5s // 20 bpm 2s
		cfg_bpm_val = bpm_input.value;
	}
}

void adjustTidalVolume()
{
	if((tidal_input.min <= tidal_input.value) && (tidal_input.max >= tidal_input.value))
	{
		FILL_ADJUST_ST.max_duration = DURATION_100CC * tidal_input.value;
	  //chris: updated to reflect 1.2L tank which takes 
  	//CFG_TANK_PRESSURE = 6.0 + tidal_input.value * 1.5; //FOR 1L
		CFG_TANK_PRESSURE = 5.0 + tidal_input.value * 0.875; //FOR 1.2L 
		cfg_tidal_val = MIN_TIDAL + tidal_input.value*100;
	}
}

void adjustHighAirway()
{
  MAX_PATIENT_PRESSURE_CFG = high_airway_input.value;
  MAX_PATIENT_PRESSURE= MAX_PATIENT_PRESSURE_CFG/70.307;
}


void updateScreen()
{
	uint8_t i = 0;
 
 while(screen_params[i] != NULL)
	{
		screen_update(screen_params[i]);
		i++;
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
      volume_flag=1;
			adjustBPM();
			adjustTidalVolume();
      adjustHighAirway();
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
      volume_flag=0;
			sov_close(TANK_SOV);
			sov_close(OUT_SOV); 
			sov_open(PATIENT_SOV);	
			break;
		case EXHALE:
			sov_close(TANK_SOV);
			sov_close(PATIENT_SOV);	
			sov_open(OUT_SOV);  
			updateScreen();
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
  screen_init(); // i2c must already been initialized before running this
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
      Serial.print(",");
      Serial.print(volume_flag);
		  Serial.print('\n');

		curr_duration_in_state += elapsed;

    sov_manage_booster();
    
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
          
          if(systeminputsensor.pressure <= MIN_INPUT_PRESSURE)
          {
            // set alarm
            digitalWrite(ALARM_LIGHT, HIGH); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, HIGH); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 3;
          }
          else 
          {
            digitalWrite(ALARM_LIGHT, LOW); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, LOW); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 0;
          }          

					break;
				case FILL_ADJUST:
					if(tanksensor.pressure >= CFG_TANK_PRESSURE)
						sov_close(TANK_SOV);

          if(systeminputsensor.pressure <= MIN_INPUT_PRESSURE)
          {
            // set alarm
            digitalWrite(ALARM_LIGHT, HIGH); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, HIGH); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 2;
          }
          else 
          {
            digitalWrite(ALARM_LIGHT, LOW); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, LOW); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 0;
          }    
            
					break;
				case INHALE:
					// \todo JB alarm
					if(patientsensor.pressure >= MAX_PATIENT_PRESSURE)
          {
            // set alarm
            digitalWrite(ALARM_LIGHT, HIGH); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, HIGH); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 1;
						change_state();
          }
          else 
          {
            digitalWrite(ALARM_LIGHT, LOW); // DM these shouls be in srv_psensor.cpp init
            digitalWrite(ALARM_BUZZER, LOW); // DM these shouls be in srv_psensor.cpp init
            alarmalarm = 0;
          }
					break;
				default:
					break;
			}
		}
	}
}
