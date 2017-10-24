#include <mbed.h>
#include "UiLcd.h"

extern void wakeup_set_interrupt(void);
extern int wakeup_read(void);
extern void wakeup_write(int value);

void uilcd_sleep_handler(void)
{
	if(sleep_timer_read()>5000&&wakeup_read()!=0)
	{
		uiLcd->lcd->printf16(0,16,"    ~_~Zzzzz     ");
		uiLcd->lcd->update();
		uiLcd->lcd->dark();

		wakeup_write(1);
		wakeup_set_interrupt();
		cpu_deep_sleep(-1);
		wakeup_write(0);
		wait_ms(1);
		wakeup_write(1);

		sleep_clear();
		
		uiLcd->lcd->bright();
		uiLcd->lcd->clear();
		uiLcd->lcd->update();
		uiLcd->Redraw=true;
		return;
	}
	return;
}
