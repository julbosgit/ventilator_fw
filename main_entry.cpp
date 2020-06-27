#include <Arduino.h>
#include "tsk_control.h"
#include "pinout.h"


void setup()
{
    delay(10);           
    Serial.begin(115200);

    pinMode(ALARM_LIGHT, OUTPUT); // DM these shouls be in srv_psensor.cpp init
    digitalWrite(ALARM_LIGHT, LOW); // DM these shouls be in srv_psensor.cpp init
    pinMode(ALARM_BUZZER, OUTPUT); // DM these shouls be in srv_psensor.cpp init
    digitalWrite(ALARM_BUZZER, LOW); // DM these shouls be in srv_psensor.cpp init
    pinMode(CS_SPI_FM2, OUTPUT); // DM these shouls be in srv_psensor.cpp init
    digitalWrite(CS_SPI_FM2, HIGH); // DM these shouls be in srv_psensor.cpp init -- high to diable for now
    pinMode(CS_SPI_FM1, OUTPUT); // DM these shouls be in srv_psensor.cpp init
    digitalWrite(CS_SPI_FM1, HIGH); // DM these shouls be in srv_psensor.cpp init -- high to diable for now
    pinMode(SOV_12V_BOOST, OUTPUT); // DM these shouls be in srv_psensor.cpp init
    digitalWrite(SOV_12V_BOOST, HIGH); // DM these shouls be in srv_psensor.cpp init -- high to diable for now

    tsk_control_init();
}  // end of setup

void loop()
{
  // \todo JB tick time? 200us? Make sure it is in line with the way tasks compute their readiness
    tsk_control();
}  // end of loop
