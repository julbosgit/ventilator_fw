#ifndef __TSK_CONTROL_H__
#define __TSK_CONTROL_H__

// in ms
#define TSK_CONTROL_PERIOD	(5)

void tsk_control_init();
void tsk_control();


typedef enum {
	IDLE,
	FILL_TANK,
	FILL_ADJUST,
	INHALE,
	EXHALE
} state_t;


typedef struct {
	state_t state;
	uint32_t max_duration; // in milliseconds
} systemstate_t;

#endif
