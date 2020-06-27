#ifndef __TSK_CONTROL_H__
#define __TSK_CONTROL_H__

// in ms
#define TSK_CONTROL_PERIOD	(5)

//Max MIN Pots
#define TIDAL_POT_HIGH   6
#define TIDAL_POT_LOW    0 
#define BPM_HIGH   20
#define BPM_LOW    12
#define LOW_AIRWAY_LOW  0
#define LOW_AIRWAY_HIGH  20
#define HIGH_AIRWAY_LOW   0
#define HIGH_AIRWAY_HIGH  60
#define PEEP_LOW    0
#define PEEP_HIGH   40

void tsk_control_init();
void tsk_control();
float convert_LPM(float raw_fm);


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
