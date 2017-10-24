#include <mbed.h>
#include <ExtIO.h>

#define POWER_ON_TIME	1000
#define POWER_OFF_TIME	1500
#define AC_ON			(extIo->IN.bit.b0!=0)

extern DigitalOut rst_oled;
DigitalIn btnPower(P4_1);

static void handler_poweroff(const void*p)
{
	extern void cpu_poweroff(void);
	Timer timer;
	while(1)
	{
		if(btnPower.read()==1)break;
		wait_ms(10);
	}
	while(1)
	{
		if(btnPower.read()==1)timer.reset();
		if(timer.read_ms()>POWER_OFF_TIME)
		{
			extern void lcd_clear(void);
			extern void lcd_update(void);
			lcd_clear();
			lcd_update();
			rst_oled.write(0);
			if(!AC_ON)cpu_poweroff();
			else 
			{
				while(1)
				{
					if(btnPower.read()==1)break;
					wait_ms(10);
				}
				extern void cpu_poweroff_reset(void);
				cpu_poweroff_reset();
			}
		}
		wait_ms(10);
	}
}

Thread thread_poweroff("power_off",handler_poweroff,NULL,osPriorityNormal,512);

