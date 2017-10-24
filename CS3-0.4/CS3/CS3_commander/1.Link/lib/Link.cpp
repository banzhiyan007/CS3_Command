#include <mbed.h>
#include "link.h"

extern void hal_link_init(uint8_t *frame_buf,int len);
extern void hal_link_tx(uint8_t *data,int len);
extern int  hal_link_rx(void);

void (*link_before_tx)(void)=NULL;
void (*link_after_tx)(void)=NULL;
void (*link_rx_cmd)(uint8_t cmd,uint8_t *data,uint16_t len)=NULL;
void (*link_after_rx)(void)=NULL;

static int link_seq=0;
uint8_t	*linkFrame;
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

int link_fail=0;
__attribute__((weak)) int link_role_main=1;
static void frame_tx(void)
{
	link_frame_start();
	if(link_before_tx)link_before_tx();
	link_frame_end();
	hal_link_tx(linkFrame,(linkFrameHead->size+15)/16*16);
}

static void frame_tx_ok(void)
{
	if(link_after_tx)link_after_tx();
}

static int frame_rx_wait(void)
{
	for(int i=0;i<100;i++)
	{
		if(hal_link_rx())
		{
			if(link_frame_remote_valid())return 1;
		}
		wait_ms(1);
	}
	return 0;
}

static void frame_rx(void)
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
		if(link_rx_cmd)link_rx_cmd(cmd,data,len);
		data+=len;
	}
	if(link_after_rx)link_after_rx();
}

static void thread_link(const void *p)
{
	linkFrame=(uint8_t*)malloc(LINK_FRAME_SIZE);
	linkFrameHead=(LINK_FRAME_HEAD*)linkFrame;
	hal_link_init(linkFrame,LINK_FRAME_SIZE);
	Timer timer;
	timer.reset();
	if(link_role_main)
	{
		wait_ms(1000);
		while(1)
		{
			frame_tx();
			if(frame_rx_wait())
			{
				//if(linkFrameHead->seq==link_seq+1)
				{
					frame_rx();
					link_seq=linkFrameHead->seq+1;
					//link_fail=0;
					frame_tx_ok();
				}
			}
			else
			{
				if(timer.read_ms()>2000)link_fail=1;
			}
		}
	}
	else
	{
		while(1)
		{
			if(frame_rx_wait())
			{
				//if(linkFrameHead->seq==link_seq+1)
				{
					frame_rx();
					link_seq=linkFrameHead->seq+1;
					frame_tx_ok();
				}
				frame_tx();
			}
			else
			{
			}
		}
	}
}

static void init_link(void)
{
	Thread *threadLink=new Thread("Link",thread_link,NULL,osPriorityHigh,512);
}

INIT_CALL("1",init_link);
