#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "srv_pot.h"


// \todo JB check if this can hang in arduino lib
void pot_update_input(potentiometer_t *pot)
{
    analogReadResolution(12);
	pot->raw_value = analogRead(pot->pin);
	pot->value = map(pot->raw_value, 0, 4095, pot->min, pot->max); //sets the value by using the pot range and mapping it to the range given for the controlled variable
}


void pot_init()
{

}

//JS update pot min max
void pot_update_cfg(potentiometer_t *pot,uint8_t min_val,uint8_t max_val){
      pot->min=min_val;
      pot->max=max_val;
      pot_update_input(pot);
  }

  
