#include <Arduino.h>
#include "srv_sov.h"




void sov_close(uint8_t id)
{
	digitalWrite(id, LOW);
}

void sov_open(uint8_t id)
{
	digitalWrite(id, HIGH);
}

// change to array of spi if needed
void sov_init()
{
	pinMode(TANK_SOV, OUTPUT);
    digitalWrite(TANK_SOV, LOW);

	pinMode(PATIENT_SOV, OUTPUT);
    digitalWrite(PATIENT_SOV, LOW);

  pinMode(OUT_SOV, OUTPUT);
  digitalWrite(OUT_SOV, LOW);
}
