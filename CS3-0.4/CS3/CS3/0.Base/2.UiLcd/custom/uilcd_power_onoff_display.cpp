#include <mbed.h>
#include "UiLcd.h"

void uilcd_power_on_display(void)
{
	char *Manufacture=reg_get_string("OEM_MANUFACTURE");
	char *ModelName=reg_get_string("OEM_MODELNAME");
	uiLcd->lcd->clear();
	uiLcd->lcd->printf16((128-strlen(Manufacture)*8)/2,4,Manufacture);
	uiLcd->lcd->printf16((128-strlen(ModelName)*8)/2,18,ModelName);
	uiLcd->lcd->compact_printf((128-strlen(VERSION)*6),24,VERSION);		
	uiLcd->lcd->update();
	wait_ms(100);
	uiLcd->beeper->beep(2500,150);
	wait_ms(100);
	uiLcd->beeper->beep(3000,150);
	wait_ms(100);
	uiLcd->lcd->clear();
}

void uilcd_power_off_display(void)
{
	uiLcd->lcd->clear();
	uiLcd->lcd->printf16((128-8*9)/2,8,uiLcd->string->POWER_OFF);
	uiLcd->lcd->update();
	wait_ms(100);
	uiLcd->beeper->beep(3000,150);
	wait_ms(100);
	uiLcd->beeper->beep(2500,150);
	wait_ms(100);
	wait_ms(300);
}
