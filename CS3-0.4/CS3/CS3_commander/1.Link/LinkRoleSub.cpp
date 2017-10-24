#include <mbed.h>
#include "link.h"

extern "C"    
{ 
#include "cs_types.h"
#include "global_macros.h"
#include "uart.h"
#include "hal_gpio.h"
#include "sys_irq.h"
}

int link_role_main=0;

static void init_link_role(void)
{
	wait_ms(500);
	hal_GpioSetPinMux(HAL_GPIO_4_4, HAL_GPIO_ALTFUNC_CFG_3);
	hal_GpioSetPinMux(HAL_GPIO_4_5, HAL_GPIO_ALTFUNC_CFG_3);
}
INIT_CALL("1",init_link_role);