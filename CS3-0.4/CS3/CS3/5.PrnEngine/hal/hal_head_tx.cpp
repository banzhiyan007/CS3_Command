#include <mbed.h>
#include "prnEngine.h"
#include "ExtIO.h"
extern "C"
{
#include "cs_types.h"
#include "gpio_config.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "sys_ctrl.h"
#include "gpio.h"
#include "spi.h"
#include "hal_spi.h"
#include "halp_sys_ifc.h"
#include "hal_timers.h"
};

int spi_dma_ch;

static void spi_frequency(int freq)
{
	int sysFreq=104000000;
	int clkDivider =  sysFreq/ ( 2 * freq);
	if (sysFreq%( 2 * freq) != 0)
	{
		clkDivider += 1;
	}

	clkDivider = (clkDivider) ? clkDivider-1 : 0;
	if(clkDivider<1)clkDivider=1;
	hwp_spi2->cfg&=~SPI_CLOCK_DIVIDER_MASK;
	hwp_spi2->cfg|=SPI_CLOCK_DIVIDER(clkDivider);
}
static void gpio_set_mux(PinName pin,HAL_GPIO_ALTFUNC_CFG_ID_T cfg)
{
	HAL_GPIO_GPIO_ID_T p=(HAL_GPIO_GPIO_ID_T)(0x10020000+((pin/256)<<3)+((pin%256)&0x07));
	hal_GpioSetPinMux(p, cfg);
}
static void init_head_tx(void)
{
	gpio_set_mux(P1_3, HAL_GPIO_ALTFUNC_CFG_2);
	gpio_set_mux(P1_0, HAL_GPIO_ALTFUNC_CFG_2);

	hwp_spi2->ctrl=SPI_ENABLE|SPI_INPUT_MODE|SPI_DI_DELAY(1)|SPI_DO_DELAY(0)|SPI_CLOCK_POLARITY|SPI_FRAME_SIZE(8-1);
	hwp_spi2->irq = 0;
	extern int sys_ifc_channel;
	spi_dma_ch=sys_ifc_channel++;
	hwp_sysIfc->std_ch[spi_dma_ch].control     =  SYS_IFC_REQ_SRC(HAL_IFC_SPI2_TX)|SYS_IFC_CH_RD_HW_EXCH|HAL_IFC_SIZE_8_MODE_MANUAL|SYS_IFC_DISABLE;
	hwp_sysIfc->std_ch[spi_dma_ch].control	   |=SYS_IFC_FLUSH;
	hwp_sysIfc->std_ch[spi_dma_ch].control	   &=~SYS_IFC_FLUSH;
	hwp_sysIfc->std_ch[spi_dma_ch].start_addr  =  (UINT32)0;
	hwp_sysIfc->std_ch[spi_dma_ch].tc          =  0;

	spi_frequency(104000000/6);
}
INIT_CALL("8",init_head_tx);

void PrnEngine::head_tx(uint8_t *data,int len)
{
	hwp_spi2->irq = 0;

	hwp_sysIfc->std_ch[spi_dma_ch].start_addr  =  (UINT32)data;
	hwp_sysIfc->std_ch[spi_dma_ch].tc          =  len;
	hwp_sysIfc->std_ch[spi_dma_ch].control	   |=  SYS_IFC_ENABLE;

	while(!(hwp_spi2->status&SPI_TX_DMA_DONE));
	while((hwp_spi2->status&SPI_TX_SPACE_MASK)!=SPI_TX_SPACE(0x10));
	while(!(hwp_spi2->status&SPI_TX_TH));
	hwp_sysIfc->std_ch[spi_dma_ch].control|=SYS_IFC_DISABLE;
}

void PrnEngine::head_latch()
{
	LATCH->write(0);
	LATCH->write(1);
}

