/*
	Arduino library for Honeywell pressure sensors
	SPI Models supported, I2C and Analog support to come
	Fletcher Bach - Dec. 2016
	MIT license
*/

#include "Arduino.h"
#include "hsc_ssc_spi.h"
#include <SPI.h>

HPS::HPS(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    _pin = pin;
}

float HPS::readPressure() {
    int whichChip = _pin;

    byte firstByte;
    byte secondByte;

    // asserting this sensor by bringing CS pin low
    digitalWrite(whichChip, LOW);

    int16_t inByte_1 = SPI.transfer(0x00);  // Read first Byte of Pressure
    int16_t inByte_2 = SPI.transfer(0x00);  // Read second Byte of Pressure
    int16_t inByte_3 = SPI.transfer(0x00);  // Read first Byte of Temperature
    int16_t inByte_4 = SPI.transfer(0x00);  // Read second Byte of Temperature

	int16_t pressure_dig = (inByte_1 & 0x3F) << 8 | inByte_2;

    float psiOutput = transferFunction(pressure_dig);

    inByte_3 = ((inByte_3 << 8) + (inByte_4 & 0xe0)) >> 5; //Shift first Temperature byte 3 left
  
    _temperature = (inByte_3 * 0.0977) - 50;

    // de-asserting this sensor by bringing CS pin high
    digitalWrite(whichChip, HIGH);
    //return (bothBytes);
    return psiOutput;
}

float HPS::getTemperature(){
  return _temperature;
}

void HPS::getStatus(){

}

float HPS::transferFunction(uint16_t dataIn) {
    float outputMax = 14746.0; // 90%2 to the 14th power (from sensor's data sheet)
    float outputMin = 1638.0; // 10%
    float pressureMax = 2.3206; // (from sensor's datasheet)
    float pressureMin = -2.3206;
    float pressure = 0.0;

    // transfer function: using sensor output to solve for pressure
    pressure = (dataIn - outputMin) * (pressureMax - pressureMin) / (outputMax - outputMin);
    pressure = pressure + pressureMin;

    return (pressure);
}
