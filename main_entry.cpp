#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "pinout.h"
#include "example_task.h"
#include "tsk_spi.h"


void setup()
{
    delay(10);           
    Serial.begin(9600);
    Wire.begin();
    tsk_spi_init();
}  // end of setup

void loop()
{
  // \todo JB tick time? 200us? Make sure it is in line with the way tasks compute their readiness
    example_task();
    tsk_spi();
}  // end of loop
