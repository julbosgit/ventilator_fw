#ifndef _SRV_PSOV_H_
#define _SRV_PSOV_H_

#include <Arduino.h>
#include "pinout.h"

void sov_init();
void sov_open(uint8_t id);
void sov_close(uint8_t id);


#endif
