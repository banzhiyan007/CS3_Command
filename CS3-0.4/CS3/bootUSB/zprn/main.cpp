#include <mbed.h>
#include <beeper.hpp>
#include "ExtIO.h"

DigitalOut rst_oled(EXT_OUT(1),1);
DigitalOut rst_ext_cpu(EXT_OUT(2),0);

Beeper beeper(P4_2);
void init_reg_value(void){}
extern "C" void boot_sector_start(void)
{
	DigitalIn ispBtn(P3_3);
	if(ispBtn.read()==1)return;
	if(isp_is_isp_reset())return;
	isp_exec_app();
}

void handler_isp(const void*p)
{
	while(1)
	{
		extern void isp(void);
		isp();
	}
}

Thread thread_isp("isp",handler_isp,NULL,osPriorityNormal,1024);

int rtos_main(void)
{
	return 0;
}

