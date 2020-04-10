#include <Arduino.h>
#include "example_task.h"


void setup()
{
    delay(10);           
    Serial.begin(9600);

    example_task_init();
}  // end of setup

void loop()
{
  // \todo JB tick time? 200us? Make sure it is in line with the way tasks compute their readiness
    example_task();
}  // end of loop
