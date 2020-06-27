#include <Arduino.h>
#include "srv_sov.h"

#define MAX_BOOST_COUNT 7 

static uint8_t isBoostOn = 0;
static uint8_t boostPeriodCounts = 0;

void sov_close(uint8_t id)
{
	digitalWrite(id, LOW);
}

void sov_open(uint8_t id)
{
//  digitalWrite(SOV_12V_BOOST, LOW); // boost on
  isBoostOn = 1;
  boostPeriodCounts = 0;
	digitalWrite(id, HIGH);
}

void sov_manage_booster()
{
  if(isBoostOn != 0)
  {
    if(boostPeriodCounts >= MAX_BOOST_COUNT)
    {
      isBoostOn = 0;
//      digitalWrite(SOV_12V_BOOST, HIGH);
    }
    boostPeriodCounts++;

  }
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
