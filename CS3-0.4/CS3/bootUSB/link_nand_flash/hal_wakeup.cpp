#include <mbed.h>
extern "C"
{
#include "cs_types.h"
#include "gpio_config.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "sys_ctrl.h"
#include "gpio.h"
#include "iomux.h"
};

static int wakeup_initialized=0;
static DigitalOut *wakeup;
static InterruptIn *wakeup_irq;
static HAL_GPIO_GPIO_ID_T pin_wakeup;
static void wakeup_init(void)
{
	if(wakeup_initialized)return;
	wakeup_initialized=1;

	wakeup=new DigitalOut(P4_3,0);
	hal_GpioSetIn(HAL_GPIO_4_3);

	wakeup_irq=new InterruptIn(P4_3);
	wakeup_irq->fall(sleep_clear);
}

INIT_CALL("9",wakeup_init);

void wakeup_set_interrupt(void)
{
	hal_GpioSetPinMux(HAL_GPIO_4_3, HAL_GPIO_ALTFUNC_CFG_7);
}

int wakeup_read(void)
{
	hal_GpioSetPinMux(HAL_GPIO_4_3, HAL_GPIO_ALTFUNC_CFG_3);
	hal_GpioSetIn(HAL_GPIO_4_3);
	int status=hal_GpioGet(HAL_GPIO_4_3);
	return status;
}

void wakeup_write(int value)
{
	if(value)
	{
		hal_GpioSetPinMux(HAL_GPIO_4_3, HAL_GPIO_ALTFUNC_CFG_3);
		hal_GpioSetIn(HAL_GPIO_4_3);
	}
	else
	{
		hal_GpioSetPinMux(HAL_GPIO_4_3, HAL_GPIO_ALTFUNC_CFG_3);
		hal_GpioSetOut(HAL_GPIO_4_3);
	}
}


