#include <mbed.h>
#include <RawUSBPrinter.h>
#include <beeper.hpp>
#include "ExtIO.h"
extern "C"    
{ 
#include "cs_types.h"
#include "global_macros.h"
#include "sys_ctrl.h"
#include "uart.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "debug_uart.h" 
#include "sys_irq.h"
#include "csw_csp.h"
#include "halp_sys_ifc.h"
#include "debug_host.h"
#include "cfg_regs.h"
#include "hal_debug.h"
#include "debug_port.h"
}

#define EXT_CPU_RESET	extIo->OUT.bit.b2
extern RawUSBPrinter *usb;
extern Beeper beeper;

static void disable_host_debug(void)
{
	hwp_debugPort->Debug_Port_Mode=0;
	hwp_debugHost->mode=0;
	hwp_debugHost->irq=0;

	hwp_configRegs->Alt_mux_select &= ~CFG_REGS_DEBUG_PORT_MASK;
	hwp_configRegs->Alt_mux_select |= CFG_REGS_DEBUG_PORT_DEBUG;
	hwp_sysCtrl->Cfg_Clk_Dbg_Div = SYS_CTRL_CLK_DBG_DIVIDER(1);
	hwp_debugPort->Debug_Port_Mode = DEBUG_PORT_MODE_CONFIG_MODE_PXTS_EXL_OFF;
}

static RawSerialRxDma *serial;
static RawSerialRxDma *debug_serial;
static void switch_to_debug_port(void)
{
	disable_host_debug();

	serial=debug_serial;

	hal_GpioSetPinMux(HAL_GPIO_4_5, HAL_GPIO_ALTFUNC_CFG_3);
	hal_GpioSetPinMux(HAL_GPIO_4_4, HAL_GPIO_ALTFUNC_CFG_3);
	hal_GpioSetPinMux(HAL_GPIO_3_6, HAL_GPIO_ALTFUNC_CFG_0);

	hal_GpioSetPinMux(HAL_GPIO_3_7, HAL_GPIO_ALTFUNC_CFG_4);
	hal_GpioSetPinMux(HAL_GPIO_4_0, HAL_GPIO_ALTFUNC_CFG_4);

	hwp_debugHost->mode=3;
}

void isp_ext_cpu5856(void)
{
	int debug_port=0;

	EXT_CPU_RESET=0;
	extIo->update();
	wait_ms(10);

	serial=new RawSerialRxDma(P3_7,P3_6,malloc(5120),5120);
	serial->set_timeout(0);

	debug_serial=new RawSerialRxDma(P4_5,P4_4,malloc(5120),5120);
	debug_serial->set_timeout(0);

	hwp_debugUart->ctrl |= DEBUG_UART_SWRX_FLOW_CTRL(1) | DEBUG_UART_SWTX_FLOW_CTRL(1);

	EXT_CPU_RESET=1;
	extIo->update();
	wait_ms(10);

	uint8_t *buf=(uint8_t*)malloc(5120);
	int len;
	Timer timer;
/*
	while(1)
	{
		printf("status:%08X\n",hwp_debugUart->status);
		printf("cause:%08X\n",hwp_debugUart->irq_cause);
		printf("rx_len:%d\n",serial->read(buf,1024));
		wait_ms(1000);
		//hwp_debugHost->mode=3;
	}
*/
	Timer resetTimer;

	while(1)
	{
		len=0;
		timer.reset();
		while(timer.read_ms()<5)
		{
			int l=usb->read(buf+len,5120-len);
			if(l)
			{
				len+=l;
				timer.reset();
			}
			if(debug_port)break;
		}

		if(len)
		{
			if(debug_port==0)
			if(len==14)
			if(memcmp(buf,"\xAD\x00\x0A\xFF\x82\xA0\x00\xA0\x01\x00\x00\x00\x00\x7C",14)==0)
			{
				debug_port=1;
				switch_to_debug_port();
			}
			serial->write(buf,len);
			resetTimer.reset();
		}

		len=0;
		timer.reset();
		while(timer.read_ms()<5)
		{
			int l=serial->read(buf+len,5120-len);
			if(l)
			{
				len+=l;
				timer.reset();
			}
			if(debug_port)break;
		}

		if(len)
		{
			usb->write(buf,len);
			resetTimer.reset();
		}

		if(debug_port==0)wait_ms(1);
		if(resetTimer.read_ms()>3000)
		{
			cpu_reset();
		}
	}
}

