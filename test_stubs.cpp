#ifdef _TEST_

#include <iostream>
#include <iomanip>
#include <stdint.h>
#include "srv_psensor.h"
#include "srv_pot.h"
#include "tsk_control.h"

extern systemstate_t *VENT_STATES[];
extern uint8_t curr_state;
extern psensor_data_t tanksensor ;
extern psensor_data_t patientsensor;
extern psensor_data_t systeminputsensor;
extern potentiometer_t bpm_input; 
extern potentiometer_t tidal_input; 


static uint32_t ticktime = 0;

static uint8_t SOV_STATE[100] = {0};
const uint8_t SOV_STATE_OPEN = 22;
const uint8_t SOV_STATE_CLOSE = 54;

CSerial Serial;

void sov_init() {}

void pot_init() {}

void ps_init(psensor_data_t) {}

uint32_t millis()
{
	ticktime += 6;
	return ticktime;
}

uint8_t ps_update_sensor(psensor_data_t *p) 
{
	switch(VENT_STATES[curr_state]->state)
	{
		case IDLE:
			p->pressure = 0;
			break;
		case FILL_TANK:
		case FILL_ADJUST:
			if((p == &tanksensor) && (SOV_STATE[TANK_SOV] != SOV_STATE_CLOSE))
				p->pressure += 0.18;
			if((p == &patientsensor) && (p->pressure >=0))
				p->pressure =0 ;
			
			break;
		case INHALE:
			if((p == &tanksensor) && (p->pressure >=0))
				p->pressure -= 0.74;
			else if(p == &patientsensor)
				p->pressure += 0.001;
			break;
		case EXHALE:
			if((p == &patientsensor) && (p->pressure >=0))
				p->pressure -= 0.0005;
			else if(p == &tanksensor)
				p->pressure =0 ;
			break;
		default:
			p->pressure=0;			
	}
	return 0;
}

void pot_update_input(potentiometer_t *p)

{
	if(p == &bpm_input)
		p->value = 17;
	else if(p == &tidal_input)
		p->value = 6;
}

void sov_open(uint8_t s)
{
//	std::cout<<"SOV "<<(int)s<<"\topen"<<std::endl;
	SOV_STATE[s] = SOV_STATE_OPEN;
}
void sov_close(uint8_t s)
{
//	std::cout<<"SOV "<<(int)s<<"\tclose"<<std::endl;
	SOV_STATE[s] = SOV_STATE_CLOSE;
}


CSerial::print(uint32_t a)
{
	std::cout<<a;
}

CSerial::print(char* a)
{
	std::cout<<a;
}

CSerial::print(float a, uint8_t n)
{
	std::cout << std::fixed << std::setprecision(n) << a;
}

CSerial::println()
{
	std::cout<<std::endl;
}

#endif