#ifndef __TSK_CONTROL_H__
#define __TSK_CONTROL_H__

// in ms
#define TSK_CONTROL_PERIOD	(5)

void tsk_control_init();
void tsk_control();


typedef enum {
	IDLE,
	FILL_TANK,
	INHALE,
	EXHALE
} state_t;


typedef struct {
	state_t state;
	uint32_t max_duration; // in milliseconds
} systemstate_t;

// \todo JB review max durations for each of these 
const systemstate_t IDLE_ST = {IDLE, 0xFFFFFFFF};
const systemstate_t FILL_TANK_ST = {FILL_TANK, 400};
const systemstate_t INHALE_ST = {INHALE, 1500};
const systemstate_t EXHALE_ST = {EXHALE, 2600}; 


#endif
