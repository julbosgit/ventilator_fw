#include <Arduino.h>
#include "tsk_control.h"


void setup()
{
    delay(10);           
    Serial.begin(115200);

    tsk_control_init();
}  // end of setup

void loop()
{
  // \todo JB tick time? 200us? Make sure it is in line with the way tasks compute their readiness
    tsk_control();
}  // end of loop
