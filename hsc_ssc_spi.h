/*
	Arduino library for Honeywell pressure sensors
	Currently just supporting SPI sensors. I2c and Analog to come.
	Fletcher Bach - Dec. 2016
	MIT License
*/

#ifndef HPS_h
#define HPS_h

#include "Arduino.h"

class HPS {
public:
	HPS(int pin);
	float readPressure();
	float getTemperature();
	void getStatus();
private:
	int _pin;
	float transferFunction(uint16_t);
  float _temperature;
};

#endif
