#include <mbed.h>
extern "C"
{
#include "cs_types.h"
#include "chip_id.h"
#include "global_macros.h"
#include "dma.h"
#include "halp_irq.h"
#include "hal_dma.h"
#include "hal_sys.h"
#include "sys_irq.h"
#include "halp_sys_ifc.h"
#include "sys_ctrl.h"
#include "debug_uart.h"
};

extern void (*g_halDmaRegistry)(void);
static void (*dma_isr)(void);

static void hal_dma_isr(void)
{
	hwp_dma->control = DMA_INT_DONE_CLEAR;
	dma_isr();
}

void hal_timeout_init(void (*isr)(void))
{
	g_halDmaRegistry=hal_dma_isr;
	dma_isr=isr;
	hal_SysRequestFreq((HAL_SYS_FREQ_USER_ID_T)17, HAL_SYS_FREQ_26M, NULL);
	hwp_sysIrq->Mask_Set = SYS_IRQ_SYS_IRQ_DMA;
}

void hal_timeout_start(int us)
{
	static uint32_t src,dst;
	if(us<200)us=200;
	uint32_t count=(us<<5)-us;
	if(count>0x3FFFF)count=0x3FFFF;
	hwp_dma->src_addr = (UINT32)&src;
	hwp_dma->dst_addr = (UINT32)&dst;
	hwp_dma->xfer_size = DMA_TRANSFER_SIZE(count);
	hwp_dma->control = DMA_DST_ADDR_MGT_CONST_ADDR | DMA_ENABLE | DMA_INT_DONE_MASK | DMA_INT_DONE_CLEAR;
	{
		// Flush the CPU write buffer.
		UINT32 unused __attribute__((unused)) = hwp_dma->control;
	}
}

