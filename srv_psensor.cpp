#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "srv_psensor.h"



/// returns
///         0 if all is fine
///         1 if chip is in command mode
///         2 if old data is being read
///         3 if a diagnostic fault is triggered in the chip
///         4 if the sensor is not hooked up
uint8_t ps_update_sensor(psensor_data_t *pdata)
{
    uint8_t i, val[4] = { 0, 0, 0, 0 };
	
	if(pdata->type.port == portspi)
	{
		digitalWrite(pdata->type.addr, LOW);
		
		for (i = 0; i <= 3; i++) {
			val[i] = SPI.transfer(0x00);
		}
		
		digitalWrite(pdata->type.addr, HIGH);
	}
	else if(pdata->type.port == porti2c0)
	{
		Wire.requestFrom(pdata->type.addr, (uint8_t) 4);
		for (i = 0; i <= 3; i++) {				
			val[i] = Wire.read();            // Wire.available()?
		}
	}
	else
	{
		Wire1.requestFrom(pdata->type.addr, (uint8_t) 4);
		for (i = 0; i <= 3; i++) {				
			val[i] = Wire1.read();            // Wire.available()?
		}
	}

    pdata->status = (val[0] & 0xc0) >> 6;  // first 2 bits from first byte
    pdata->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    pdata->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;
	pdata->pressure = 1.0 * (pdata->bridge_data - pdata->type.outmin) * (pdata->type.pmax - pdata->type.pmin) / (pdata->type.outmax - pdata->type.outmin) + pdata->type.pmin;
    pdata->temperature = (pdata->temperature_data * 0.0977) - 50;
	
    if ( pdata->temperature_data == 65535 ) 
		return 4;
    
	return pdata->status;
}

// change to array of spi if needed
void ps_init(const psensor_t spisensor)
{
    SPI.begin();
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
	pinMode(spisensor.addr, OUTPUT);
    digitalWrite(spisensor.addr, HIGH);
	
	Wire1.begin();
    Wire1.setClock(50000); // JB trying out different frequency for M3200 cause I have issues with it
	
	Wire.begin();
}

/*
uint8_t ps_convert(psensor_data_t *pdata, const psensor_t sensor)
{
    pdata->pressure = 1.0 * (pdata->bridge_data - sensor.outmin) * (sensor.pmax - sensor.pmin) / (sensor.outmax - sensor.outmin) + sensor.pmin;
    pdata->temperature = (pdata->temperature_data * 0.0977) - 50;
    return 0;
}
*/
