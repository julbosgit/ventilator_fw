#ifdef _TEST_
#ifndef STUBS_H
#define STUBS_H

#include <stdint.h>

#define NULL 0

#define A0 	0
#define A2	2

#define static 

uint32_t millis();

class CSerial {
	public:
	static print(uint32_t);
	static print(char*);
	static print(float, uint8_t);
	static println();
};

extern CSerial Serial;

#endif

#endif
