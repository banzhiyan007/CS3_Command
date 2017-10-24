#include <mbed.h>

extern void wakeup_set_interrupt(void);
extern int wakeup_read(void);
extern void wakeup_write(int value);

int SleepHandler(void)
{
	if(sleep_timer_read()>100&&wakeup_read()!=0)
	{
		wakeup_set_interrupt();
		cpu_deep_sleep(-1);
		wakeup_write(0);
		wait_ms(1);
		wakeup_write(1);
		sleep_clear();
		return 1;
	}
	return 0;
}