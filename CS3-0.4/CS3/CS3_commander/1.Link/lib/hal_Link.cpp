#include <mbed.h>

extern "C"    
{ 
#include "cs_types.h"
#include "global_macros.h"
#include "uart.h"
#include "sys_irq.h"
}

static RawSerialRxDma *serial;

void hal_link_init(uint8_t *frame_buf,int len)
{
	serial=new RawSerialRxDma(P3_7,P3_6,frame_buf,len);
	serial->set_speed(4333333);
	hwp_sysIrq->Mask_Clear = SYS_IRQ_SYS_IRQ_UART;
}

void hal_link_tx(uint8_t *data,int len)
{
	serial->write_dma(data,len);
}

int hal_link_rx(void)
{
	uint32_t irq_cause=hwp_uart->irq_cause;
	hwp_uart->irq_cause=irq_cause;
	hwp_uart->status=1;

	if((irq_cause&UART_RX_DMA_DONE_U)||(irq_cause&UART_RX_DMA_TIMEOUT_U))
	{
		void raw_serial_rx_dma_rx_restart(int port);
		raw_serial_rx_dma_rx_restart(1);
		return 1;
	}
	return 0;
}
