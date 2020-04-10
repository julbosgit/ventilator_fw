#include <Arduino.h>
#include "srv_psensor.h"


// JB \todo EXAMPLE FOR NOW
static psensor_data_t tanksensor = { 0,0,0,0,0,SENSOR_M3200};
static psensor_data_t flowsensor = { 0,0,0,0,0,SENSOR_HSC};
static psensor_data_t patientsensor = { 0,0,0,0,0,SENSOR_SSC};

static psensor_data_t *sensors[] = {&tanksensor, &flowsensor, &patientsensor, NULL};


static const uint32_t task_interval_ms = 5000;
static uint32_t task_prev = 0; 


void example_task_init()
{
  // JB \todo change to array - must change function
  ps_init(SENSOR_HSC);
}

void example_task()
{
  uint8_t el;
	uint32_t now = millis();

    if ((now - task_prev > task_interval_ms) && (Serial.available() <= 0)) {
        task_prev = now;
        
		Serial.println("******* TIME ");
		Serial.println(now);
		for(uint8_t i=0 ; sensors[i] != NULL ; i++)
		{
			el = ps_update_sensor(sensors[i]);
			// for some reason my chip triggers a diagnostic fault
			// on 50% of powerups without a notable impact 
			// to the output values.
			if ( el == 4 ) {
				Serial.println("err sensor missing");
			} else 
			{
				if ( el == 3 ) {
					Serial.print("err diagnostic fault ");
					Serial.println(sensors[i]->status, BIN);
				}
				if ( el == 2 ) {
					// if data has already been feched since the last
					// measurement cycle
					Serial.print("warn stale data ");
					Serial.println(sensors[i]->status, BIN);
				}
				if ( el == 1 ) {
					// chip in command mode
					// no clue how to end up here
					Serial.print("warn command mode ");
					Serial.println(sensors[i]->status, BIN);
				}
				Serial.print(i);
				Serial.print(" status      ");
				Serial.println(sensors[i]->status, BIN);
				Serial.print("pressure (unit may difer) ");
				Serial.println(sensors[i]->pressure,4);
				Serial.print("temperature (degC) ");
				Serial.println(sensors[i]->temperature,4);
				Serial.println("");
			}
		}
	}
}
