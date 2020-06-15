#ifndef _SRV_PSENSOR_H_
#define _SRV_PSENSOR_H_

#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif
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
	const uint8_t extra_cmd;
} psensor_t ;

typedef struct {
    uint8_t status;             // 2 bit
    uint16_t bridge_data;       // 14 bit
    uint16_t temperature_data;  // 11 bit
  	float pressure;
  	float temperature;
  	const psensor_t type;
	  const psensor_port_t port;
	  const uint8_t addr; // Use chip select pin number for spi
   char name[10];
} psensor_data_t;

const psensor_t SENSOR_M3200 = { 0, 100, 1000, 15000, 1 };
const psensor_t SENSOR_SSC = {-5, 5, 0x666, 0x399A, 0 }; // SSCDRRN005ND2A5
const psensor_t SENSOR_HSC = {-2.3206, 2.3206, 1638, 14746, 0 };  

void ps_init(const psensor_data_t spisensor);
uint8_t ps_update_sensor(psensor_data_t *pdata);

#endif
