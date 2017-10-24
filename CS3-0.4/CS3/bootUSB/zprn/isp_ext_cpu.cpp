#include <mbed.h>
#include <beeper.hpp>
#include "ExtIO.h"

extern Beeper beeper;

#define RST_EXT_CPU1	extIo.OUT.bit.b2

void isp_ext_cpu(int cpu_id)
{
	beeper.beep(3000,100);
	if(cpu_id==1)
	{
		/*
		RST_EXT_CPU1=0;
		extIo.update();
		wait_ms(10);
		RST_EXT_CPU1=1;
		extIo.update();
		*/
		void isp_ext_cpu5856(void);
		isp_ext_cpu5856();
	}
}

