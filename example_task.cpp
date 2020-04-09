#include <Arduino.h>
#include <Wire.h>
#include "hsc_ssc_i2c.h"

#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0x666        // 10%
#define OUTPUT_MAX 0x399A       // 90% of 2^14 - 1
#define PRESSURE_MIN 0.0        // min is 0 for sensors that give absolute values
#define PRESSURE_MAX 206842.7   // 30psi (and we want results in pascals)

static const uint32_t task_interval_ms = 5000;
static uint32_t task_prev = 0; 


void example_task()
{
	uint32_t now = millis();
	float p, t;
    struct cs_raw ps;
    uint8_t el;

    if ((now - task_prev > task_interval_ms) && (Serial.available() <= 0)) {
        task_prev = now;
        el = ps_get_raw(SLAVE_ADDR, &ps);
        // for some reason my chip triggers a diagnostic fault
        // on 50% of powerups without a notable impact 
        // to the output values.
        if ( el == 4 ) {
            Serial.println("err sensor missing");
        } else {
            if ( el == 3 ) {
                Serial.print("err diagnostic fault ");
                Serial.println(ps.status, BIN);
            }
            if ( el == 2 ) {
                // if data has already been feched since the last
                // measurement cycle
                Serial.print("warn stale data ");
                Serial.println(ps.status, BIN);
            }
            if ( el == 1 ) {
                // chip in command mode
                // no clue how to end up here
                Serial.print("warn command mode ");
                Serial.println(ps.status, BIN);
            }
            Serial.print("status      ");
            Serial.println(ps.status, BIN);
            Serial.print("bridge_data ");
            Serial.println(ps.bridge_data, DEC);
            Serial.print("temp_data   ");
            Serial.println(ps.temperature_data, DEC);
            Serial.println("");
            ps_convert(ps, &p, &t, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);
            Serial.print("pressure    (Pa) ");
            Serial.println(p,2);
            Serial.print("temperature (dC) ");
            Serial.println(t,2);
            Serial.println("");
        }
    }
}