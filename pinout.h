#ifndef __PINOUT_H__
#define __PINOUT_H__

//Settings from Potentiometer
#define BPM_POT   (A0)
#define LOW_AIRWAY_ALERT_POT    (A1) //DM
#define TIDAL_POT (A2)
#define HIGH_AIRWAY_ALERT_POT    (A3) //DM
#define O2_PERCENT_POT    (A4) //DM

//Flowmeters
#define CS_SPI_FM2 (4) //DM
#define CS_SPI_FM1 (9) //DM

//Pressure Sensors
#define CS_SPI_PSIG4 (5) 

//SOV
#define TANK_SOV    (10)
#define PATIENT_SOV (3)
#define OUT_SOV     (11)

//Power
#define SOV_12V_BOOST (2) //DM
#define POWER_M3200   (7)

//Alarms
#define ALARM_LIGHT (8) //DM
#define ALARM_BUZZER (12) //DM


#endif
