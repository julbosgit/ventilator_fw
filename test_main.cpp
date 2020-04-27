#ifdef _TEST_

#include <stdint.h>
#include "tsk_control.h"

#define DURATION_MINUTES	1
#define DURATION_MS		(DURATION_MINUTES*60*1000)/6


int main()
{
	tsk_control_init();
	
	for(int i=0 ; i<DURATION_MS ; i++)
		tsk_control();
	return 0;
}

#endif