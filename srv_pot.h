#ifndef _SRV_POT_H_
#define _SRV_POT_H_

#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif
#include "pinout.h"

typedef struct {
	uint8_t min;
	uint8_t max;
	const uint8_t pin;
	uint16_t value;
	uint16_t raw_value;
} potentiometer_t;

void pot_init();
void pot_update_input(potentiometer_t *pot);
void pot_update_cfg(potentiometer_t *pot,uint8_t min_val,uint8_t max_val);   //JS Update pot min max

#endif
