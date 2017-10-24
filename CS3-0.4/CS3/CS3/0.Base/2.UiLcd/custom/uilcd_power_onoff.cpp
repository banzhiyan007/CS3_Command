#include <mbed.h>
#include "UiLcd.h"

__attribute__((weak)) void uilcd_battery_display_poweroff(void){}

void uilcd_power_on_check(void)
{
	uiLcd->lcd->clear();
	uiLcd->lcd->update();

	extern int reset_direct_start;
	if(reset_direct_start)return;

	int count=0;
	while(1)
	{			
		for(int i=0;i<10;i++)
		{
			uiLcd->btnPower->handler();
			if(!uiLcd->btnPower->active())count=0;
			count++;
			if(count>=(1000/50))return;
			wait_ms(50);
		}
		uilcd_battery_display_poweroff();
	}
}

bool uilcd_power_off_check(void)
{
	static int can_power_off=0;
	if(!can_power_off)
	{
		if(!uiLcd->btnPower->active())
		{
			can_power_off=1;
		}
	}
	else
	{
		if(uiLcd->btnPower->down_time()>1500)return true;
	}
	return false;
}

void uilcd_power_off(void)
{
	uiLcd->lcd->clear();
	uiLcd->lcd->update();
	board->out_lcd_reset->write(0);
	board->out_ext_cpu_reset->write(0);

	if(!board->ac_ok->active())
	{
		extern void cpu_poweroff(void);
		cpu_poweroff();
	}
	else 
	{
		while(1)
		{
			uiLcd->btnPower->handler();
			if(uiLcd->btnPower->active()==0)break;
			wait_ms(10);
		}
		extern void cpu_poweroff_reset(void);
		cpu_poweroff_reset();
	}
	while(1)wait_ms(100);
}