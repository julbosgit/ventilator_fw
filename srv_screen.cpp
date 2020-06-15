#include <Arduino.h>
#include "srv_screen.h"
#include "LiquidCrystal_PCF8574.h"


static LiquidCrystal_PCF8574 lcd(0x27); 

void screen_init()
{
  lcd.begin(20, 4); // initialize the lcd
 	lcd.setBacklight(255);
  lcd.home();
	lcd.noCursor();
	lcd.noBlink();
}


void screen_update(screen_param_t *p)
{
	char v[20];
	if(p == NULL)
		return;
	
	lcd.setCursor(p->screen_col, p->screen_row);
	switch(p->t)
	{
		case integer: 
			sprintf(v, "%d", *((uint16_t*)p->val));
			lcd.print(v); 
			break;
		case string: lcd.print(((char*)p->val)); break;
		default: lcd.print("-");
	}


    // lcd.clear();
    // lcd.print("I will try");
	// lcd.setCursor(0,1);
	// lcd.print("at least LCD works");
	// lcd.setCursor(0,2);

  }
