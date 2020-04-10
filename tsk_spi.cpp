#include <Arduino.h>
#include <SPI.h>
#include "hsc_ssc_spi.h"
#include "pinout.h"

/*
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
*/

static const uint32_t task_interval_ms = 2000;
static uint32_t task_prev = 0; 
static HPS hps(CS_SPI_PSIG4);

void tsk_spi_init()
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
}


void tsk_spi()
{
	uint32_t now = millis();
  float spipressure;
  if ((now - task_prev > task_interval_ms) && (Serial.available() <= 0)) 
	{
        task_prev = now;
        spipressure = hps.readPressure();
        Serial.print("SPI pressure (psi) ");
        Serial.println(spipressure,4);
        Serial.print("SPI temperature (degC) ");
        Serial.println(hps.getTemperature());
        Serial.println("");
  }
}
