#ifndef SPI_RAM_HPP
#define SPI_RAM_HPP

#include "Ram.hpp"

class SpiRam:public Ram
{
private:
	SPI _spi;
	void get_id(void)
	{
		_spi.select(0);
		_spi.write(0x9F);
		_spi.write(0x00);
		_spi.write(0x00);
		_spi.write(0x00);
		uint8_t buf[8];
		_spi.read((char*)buf,8);
		_spi.select(1);
		id=buf[0]*256+buf[1];
		if(id==0x0D5D)
		{
			uint8_t capacity=((buf[2]>>5)&0x7);
			if(capacity==1)size=8*1024*1024;
		}
		else size=0;
	}
	bool var_locked;
	uint32_t malloc_addr;
public:
	bool *locked;
	uint16_t id;
public:
	SpiRam(PinName mosi, PinName miso, PinName sclk, PinName cs):_spi(mosi,miso,sclk,cs)
	{
		malloc_info=NULL;
		var_locked=false;
		locked=&var_locked;
		malloc_addr=1;
		_spi.frequency(104000000);
		get_id();
		get_id();
	}
	void write(uint32_t addr,uint8_t *data,int len)
	{
		if(len==0)return;
		while(*locked)wait_ms(1);
		*locked=true;
		_spi.select(0);
		char buf[4];
		buf[0]=0x02;
		buf[1]=(addr>>16)&0xFF;
		buf[2]=(addr>>8)&0xFF;
		buf[3]=(addr>>0)&0xFF;
		_spi.write(buf,4);
		_spi.write((char*)data,len);
		_spi.select(1);
		*locked=false;
	}
	void read(uint32_t addr,uint8_t *data,int len)
	{
		if(len==0)return;
		while(*locked)wait_ms(1);
		*locked=true;
		_spi.select(0);
		char buf[5];
		buf[0]=0x0b;
		buf[1]=(addr>>16)&0xFF;
		buf[2]=(addr>>8)&0xFF;
		buf[3]=(addr>>0)&0xFF;
		buf[4]=0x00;
		_spi.write(buf,5);
		_spi.read((char*)data,len);
		_spi.select(1);
		*locked=false;
	}
	void *malloc(int bytes,const char *name="")
	{
		RAM_MALLOC_INFO *info=(RAM_MALLOC_INFO *)::malloc(sizeof(RAM_MALLOC_INFO));
		info->pNext=malloc_info;
		malloc_info=info;

		uint32_t addr;
		if(malloc_addr+bytes>=size)
		{
			addr=NULL;
		}
		else
		{
			addr=malloc_addr;
			malloc_addr+=bytes;
			free_size=size-malloc_addr;
		}
		info->name=name;
		info->addr=addr;
		info->size=bytes;
		return (void*)addr;
	}
};

#endif
