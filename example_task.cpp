#include <Arduino.h>
#include <Wire.h>
#include "hsc_ssc_i2c.h"

#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0x666        // 10%
#define OUTPUT_MAX 0x399A       // 90% of 2^14 - 1
// UPDATE FOR EACH SENSOR - 
// SSCDRRN005ND2A5
#define PRESSURE_MIN -5.0       // in H2O
#define PRESSURE_MAX 5.0        // in H2O
// M32JM-000105-100PG
//#define PRESSURE_MIN 0       // psi
//#define PRESSURE_MAX 100        // psi

static const uint32_t task_interval_ms = 2000;
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
            Serial.print("I2C status      ");
            Serial.println(ps.status, BIN);
            ps_convert(ps, &p, &t, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);
            Serial.print("I2C pressure    (in H2O) ");
            Serial.println(p,4);
            Serial.print("I2C temperature (degC) ");
            Serial.println(t,4);
            Serial.println("");
        }
    }
}
