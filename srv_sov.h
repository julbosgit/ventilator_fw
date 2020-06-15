#ifndef _SRV_PSOV_H_
#define _SRV_PSOV_H_

#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif
#include "pinout.h"

void sov_init();
void sov_open(uint8_t id);
void sov_close(uint8_t id);
void sov_manage_booster();

#endif
