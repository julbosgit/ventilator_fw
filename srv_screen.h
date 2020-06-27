#ifndef _SRV_SCREEN_H
#define _SRV_SCREEN_H

#ifndef _TEST_
#include <Arduino.h>
#else
	#include "test_stubs.h"
#endif


typedef enum {
	integer,string,flag,floating
} screen_param_type_t;

typedef struct {
	void* val;
	screen_param_type_t size;
	uint8_t screen_row;
	uint8_t screen_col;
	uint8_t screen_size;
	screen_param_type_t t;
} screen_param_t;

void screen_init();
void screen_update(screen_param_t *p);

#endif
