#ifndef _SRV_PSENSOR_H_
#define _SRV_PSENSOR_H_

#include <Arduino.h>
#include "pinout.h"


typedef enum  {
	portspi,
	porti2c0,
	porti2c1
} psensor_port_t;

typedef struct {
	const float pmin;
	const float pmax;
	const uint16_t outmin;
	const uint16_t outmax;
	const psensor_port_t port;
	const uint8_t addr; // Use chip select pin number for spi
} psensor_t ;

typedef struct {
    uint8_t status;             // 2 bit
    uint16_t bridge_data;       // 14 bit
    uint16_t temperature_data;  // 11 bit
  	float pressure;
  	float temperature;
  	const psensor_t type;
} psensor_data_t;


const psensor_t SENSOR_M3200 = { 0, 100, 1000, 15000, porti2c1, 0x28 };
const psensor_t SENSOR_SSC = {-5, 5, 0x666, 0x399A, porti2c0, 0x28 }; // SSCDRRN005ND2A5
const psensor_t SENSOR_HSC = {-2.3206, 2.3206, 1638, 14746, portspi, CS_SPI_PSIG4 };  

void ps_init(const psensor_t spisensor);
uint8_t ps_update_sensor(psensor_data_t *pdata);

#endif
