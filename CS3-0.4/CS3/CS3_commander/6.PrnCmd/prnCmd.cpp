#include <mbed.h>
#include "prnEngine.h"
#include "EngineCmd.h"
#include "prnCmd.h"

int LINE_DOT_MAX=832;

PrnCmd *prnCmd;
static void init_prn_cmd(void)
{
	prnCmd=new PrnCmd;
}
INIT_CALL("6",init_prn_cmd);

static const uint8_t U8_REVERSE[256]=
{
	0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
	0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
	0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
	0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
	0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
	0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
	0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
	0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
	0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
	0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
	0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
	0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
	0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
	0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
	0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
	0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

static const uint8_t U8_DOT_COUNT[256]=
{
	0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03,0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04,
	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04,0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,
	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04,0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04,0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,0x04,0x05,0x05,0x06,0x05,0x06,0x06,0x07,
	0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04,0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,0x04,0x05,0x05,0x06,0x05,0x06,0x06,0x07,
	0x02,0x03,0x03,0x04,0x03,0x04,0x04,0x05,0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,
	0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,0x04,0x05,0x05,0x06,0x05,0x06,0x06,0x07,
	0x03,0x04,0x04,0x05,0x04,0x05,0x05,0x06,0x04,0x05,0x05,0x06,0x05,0x06,0x06,0x07,
	0x04,0x05,0x05,0x06,0x05,0x06,0x06,0x07,0x05,0x06,0x06,0x07,0x06,0x07,0x07,0x08,
};

static void cmd_put(uint8_t cmd,uint16_t param,const void *buf,int len)
{
	uint8_t head[4];
	head[0]=0x1F;
	head[1]=cmd;
	head[2]=param%256;
	head[3]=param/256;
	head[4]=len%256;
	head[5]=len/256;
	while(prnEngine->fifo_rx->free_length()<6+len)wait_ms(1);
	prnEngine->fifo_rx->write(head,6);
	prnEngine->fifo_rx->write(buf,len);
}

PrnCmd::PrnCmd()
{

}

void PrnCmd::feed(uint16_t linecount)
{
	cmd_put(LINE_CMD_FEED,linecount,"\x00",1);
}

void PrnCmd::feed_backward(uint16_t linecount)
{
	cmd_put(LINE_CMD_FEED_BACKWARD,linecount,"\x00",1);
}

void PrnCmd::goto_mark_left(uint16_t linecount)
{
	char buf[1];
	buf[0]=MARK_L;
	cmd_put(LINE_CMD_FEED,linecount,buf,1);
}

void PrnCmd::goto_mark_right(uint16_t linecount)
{
	char buf[1];
	buf[0]=MARK_R|MARK_R1;
	cmd_put(LINE_CMD_FEED,linecount,buf,1);
}

void PrnCmd::goto_mark_label(uint16_t linecount)
{
	char buf[1];
	buf[0]=MARK_LABEL;
	cmd_put(LINE_CMD_FEED,linecount,buf,1);
}

void PrnCmd::goto_mark_all(uint16_t linecount)
{
	char buf[1];
	buf[0]=MARK_L|MARK_R|MARK_R1|MARK_LABEL;
	cmd_put(LINE_CMD_FEED,linecount,buf,1);
}

void PrnCmd::delay(uint16_t ms)
{
	cmd_put(LINE_CMD_DELAY,ms,NULL,0);
}

void PrnCmd::print_line(void *pLineData,int Len)
{
	uint8_t *data=(uint8_t *)pLineData;
	uint16_t dot_count=0;
	for(int i=0;i<Len;i++)
	{
		dot_count+=U8_DOT_COUNT[data[i]];
		data[i]=U8_REVERSE[data[i]];
	}
	
	#define HEAT_TIME_25	600
	#define r_R				700

	uint32_t heat_time=0;
	int darkness=2;
	heat_time=(dot_count+r_R)*(dot_count+r_R)*HEAT_TIME_25/(1024*512)+20;
//	heat_time=heat_time*(darkness+2)/(2+2);
	if(heat_time>0xFFFF)heat_time=0xFFFF;

	cmd_put(LINE_CMD_PRINT,heat_time,pLineData,Len);
}

void PrnCmd::set_darkness(int darkness)
{
	cmd_put(LINE_CMD_DARKNESS,darkness,NULL,0);
}
void PrnCmd::set_speed(int speed)
{
	cmd_put(LINE_CMD_SPEED,speed,NULL,0);
}
