#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "pinout.h"
#include "example_task.h"


void setup()
{
    delay(10);           
    Serial.begin(9600);
    Wire.begin();
}  // end of setup

void loop()
{
    example_task();
}  // end of loop
