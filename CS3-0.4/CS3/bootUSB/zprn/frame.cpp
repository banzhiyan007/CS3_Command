#include "mbed.h"
#include "RawUSBPrinter.h"
#include "Beeper.hpp"

extern RawUSBPrinter *usb;
extern Beeper beeper;

void frame_write(uint8_t *buf,int len)
{
    usb->write(buf,len);
}

int frame_read(uint8_t *buffer,int length,int timeout)
{
    Timer timer;
    int pos=0;
    while(timer.read_ms()<timeout)
    {
        int len=length-pos;
        int l;
		l=usb->read(buffer+pos,len);
        pos+=l;
        if(pos>=length)return 1;
        wait_ms(1);
    }
    return 0;
}

void frame_send(unsigned char type,uint8_t *buf,unsigned short len)
{
	uint8_t head[6+32];
	head[0]=0x1f;
	head[1]=0xAA;
	head[2]=(len+2)%256;
	head[3]=(len+2)/256;
    head[4]=type;
    head[5]=type^0xff;
    if(len>32)return;
    memcpy(head+6,buf,len);
	frame_write(head,6+len);
}
int  frame_wait(unsigned char *type,void *buf,unsigned short *len,int max_size,int timeout)
{
	uint8_t head[4];
	uint16_t pack_size;
	uint8_t frame_type=0;
	if(frame_read(head,1,timeout)==0)return 0;
	if(head[0]!=0x1f)return 0;
	if(frame_read(head,1,timeout)==0)return 0;

	if(head[0]==0xAA)frame_type=0x01;
	else if(head[0]==0x4E)frame_type=0x02;
	else return 0;

    if(frame_type==0x01)
    {
        if(frame_read(head,2,timeout)==0)return 0;
        memcpy(&pack_size,&head[0],2);
        *len = pack_size-2;
        if(*len>max_size)return 0;

        if(frame_read(head,2,timeout)==0)return 0;
        if(head[0]!=(head[1]^0xFF))return 0;
        *type = head[0];
        uint16_t total = *len;
        while(total)
        {
            uint16_t r=total;
            if(r>256)r=256;
            if(frame_read((uint8_t*)buf+*len-total,r,timeout)==0)return 0;
            total-=r;
        }
    }
    else if(frame_type==0x02)
    {
        if(frame_read(head,2,timeout)==0)return 0;
        memcpy(&pack_size,&head[0],2);
        *len = pack_size;
        if(*len>max_size)return 0;

        uint16_t total = *len;
        while(total)
        {
            uint16_t r=total;
            if(r>256)r=256;
            if(frame_read((uint8_t*)buf+*len-total,r,timeout)==0)return 0;
            total-=r;
        }
    }
	return frame_type;
}

