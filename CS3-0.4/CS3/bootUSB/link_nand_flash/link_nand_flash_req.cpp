#include <mbed.h>

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
#include "cos.h"
}

extern DigitalOut rst_ext_cpu;

extern void hal_link_init(uint8_t *frame_buf,int len);
extern void hal_link_tx(uint8_t *data,int len);
extern int  hal_link_rx(void);

#define LINK_FRAME_SIZE	(4096+512)
static uint8_t *linkFrame;
static int link_seq=0;
struct LINK_FRAME_HEAD
{
	uint32_t beginFlag;
	uint16_t seq;
	uint16_t size;
}__attribute__((packed));
LINK_FRAME_HEAD *linkFrameHead;

static void link_frame_start(void)
{
	linkFrameHead->beginFlag=CPU_ID32;
	linkFrameHead->seq=link_seq;
	linkFrameHead->size=sizeof(LINK_FRAME_HEAD);
}

void link_cmd_add(uint8_t cmd,void *data,uint16_t len)
{
	if(linkFrameHead->size+3+len+4>LINK_FRAME_SIZE)return;
	linkFrame[linkFrameHead->size++]=cmd;
	linkFrame[linkFrameHead->size++]=len%256;
	linkFrame[linkFrameHead->size++]=len/256;
	memcpy(&linkFrame[linkFrameHead->size],data,len);
	linkFrameHead->size+=len;
}

static void link_frame_end(void)
{
	uint32_t end_flag=CPU_ID32^0xFFFFFFFF;
	memcpy(&linkFrame[linkFrameHead->size],&end_flag,4);
	linkFrameHead->size+=4;
}

static int link_frame_remote_valid(void)
{
	uint32_t beginFlag;
	uint16_t flen;
	uint32_t endFlag;

	memcpy(&endFlag,&linkFrame[linkFrameHead->size-4],4);

	if(linkFrameHead->beginFlag==CPU_ID32)return 0;
	if(endFlag!=(linkFrameHead->beginFlag^0xFFFFFFFF))return 0;
	return 1;
}

static int frame_rx_wait(void)
{
	for(int i=0;i<1000;i++)
	{
		if(hal_link_rx())
		{
			if(link_frame_remote_valid())return 1;
		}
		wait_ms(1);
	}
	return 0;
}

enum
{
	CMD_NAND_READ_INFO=0x80,
	CMD_NAND_WRITE_INFO=0x81,
	CMD_NAND_BLOCK_INDEX=0x82,
	CMD_NAND_WRITE_DATA=0x83,
	CMD_NAND_WRITE_FCS=0x84,
	CMD_NAND_READ_FCS=0x85,
};

int link_nand_flash_block_size=0;
int link_nand_flash_block_count=0;
int link_nand_flash_req_info(void)
{
	extern void wakeup_set_interrupt(void);
	extern int wakeup_read(void);
	extern void wakeup_write(int value);

	wakeup_write(0);
	rst_ext_cpu.write(1);

	wait_ms(1000);

	linkFrame=(uint8_t*)malloc(LINK_FRAME_SIZE);
	linkFrameHead=(LINK_FRAME_HEAD *)linkFrame;
	hal_link_init(linkFrame,LINK_FRAME_SIZE);
	frame_rx_wait();

	link_frame_start();
	link_cmd_add(CMD_NAND_READ_INFO,NULL,0);
	link_frame_end();
	hal_link_tx(linkFrame,(linkFrameHead->size+15)/16*16);
	
	if(frame_rx_wait())
	{
		uint16_t flen;
		memcpy(&flen,&linkFrame[4+2],2);
		uint8_t *frameEnd=(uint8_t*)(linkFrame+flen-4);
		uint8_t *data=(uint8_t*)(&linkFrame[sizeof(LINK_FRAME_HEAD)]);
		while(data<frameEnd)
		{
			uint8_t cmd;
			uint16_t len;
			cmd=*data++;
			memcpy(&len,data,2);
			data+=2;
			{
				if(cmd==CMD_NAND_WRITE_INFO)
				{
					memcpy(&link_nand_flash_block_size,&data[0],4);
					memcpy(&link_nand_flash_block_count,&data[4],4);
					return 1;
				}
			}
			data+=len;
		}
	}
	return 0;
}

int link_nand_flash_req_write(void *data,int block,uint16_t fcs)
{
	link_frame_start();
	link_cmd_add(CMD_NAND_BLOCK_INDEX,&block,4);
	link_cmd_add(CMD_NAND_WRITE_DATA,data,link_nand_flash_block_size);
	link_frame_end();
	hal_link_tx(linkFrame,(linkFrameHead->size+15)/16*16);

	if(frame_rx_wait())
	{
		uint16_t flen;
		memcpy(&flen,&linkFrame[4+2],2);
		uint8_t *frameEnd=(uint8_t*)(linkFrame+flen-4);
		uint8_t *data=(uint8_t*)(&linkFrame[sizeof(LINK_FRAME_HEAD)]);
		while(data<frameEnd)
		{
			uint8_t cmd;
			uint16_t len;
			cmd=*data++;
			memcpy(&len,data,2);
			data+=2;
			{
				if(cmd==CMD_NAND_WRITE_FCS)
				{
					if(memcmp(&data[0],&fcs,2)==0)return 1;
				}
			}
			data+=len;
		}
	}
	return 0;
}

int link_nand_flash_req_read_fcs(int block,uint16_t *fcs)
{
	link_frame_start();
	link_cmd_add(CMD_NAND_READ_FCS,&block,4);
	link_frame_end();
	hal_link_tx(linkFrame,(linkFrameHead->size+15)/16*16);

	if(frame_rx_wait())
	{
		uint16_t flen;
		memcpy(&flen,&linkFrame[4+2],2);
		uint8_t *frameEnd=(uint8_t*)(linkFrame+flen-4);
		uint8_t *data=(uint8_t*)(&linkFrame[sizeof(LINK_FRAME_HEAD)]);
		while(data<frameEnd)
		{
			uint8_t cmd;
			uint16_t len;
			cmd=*data++;
			memcpy(&len,data,2);
			data+=2;
			{
				if(cmd==CMD_NAND_WRITE_FCS)
				{
					memcpy(fcs,&data[0],2);
					return 1;
				}
			}
			data+=len;
		}
	}
	return 0;
}