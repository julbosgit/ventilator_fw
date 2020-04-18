#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "srv_pot.h"



void pot_update_input(potentiometer_t *pot)
{
    analogReadResolution(12);
	pot->raw_value = analogRead(pot->pin);
	pot->value = map(pot->raw_value, 0, 4095, pot->min, pot->max);
}


void pot_init()
{

}
