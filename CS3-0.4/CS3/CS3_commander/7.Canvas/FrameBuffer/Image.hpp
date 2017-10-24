#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <SpiFlash.h>

class Image
{
public:
	int Width;
	int Height;
	int bbw;
	void set_size(int width,int height)
	{
		Width=width;
		Height=height;
		bbw=(Width+7)/8;
	}
	virtual uint8_t *read_line_data(int y,uint8_t *data,int data_len,int data_offset=0)=0;
	virtual void write_line_data(int y,uint8_t *data,int data_len,int data_offset=0)=0;
};

class ImageBlock:public Image
{
private:
	uint8_t *pData;
public:
	ImageBlock(Block *block)
	{
		pData=block->pData;
		set_size(block->Width,block->Height);
	}
	uint8_t* read_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
		uint8_t *ptr=pData+y*bbw+data_offset;
		return ptr;
	}
	void write_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
		uint8_t *ptr=pData+y*bbw+data_offset;
		memcpy(ptr,data,data_len);
	}
};

class ImageRam:public Image
{
public:
	Ram *ram;
	uint32_t data_addr;
public:
	ImageRam(Ram *ram,int width,int height)
	{
		this->ram=ram;
		set_size(width,height);
		this->data_addr=(uint32_t)ram->malloc(bbw*height,"ImageRam");
	}
	uint8_t* read_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
		ram->read(data_addr+bbw*y+data_offset,data,data_len);
		return data;
	}
	void write_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
		ram->write(data_addr+bbw*y+data_offset,data,data_len);
	}
};

class ImageSpiFlash:public Image
{
public:
	SpiFlash *spi_flash;
	uint32_t data_addr;
public:
	ImageSpiFlash(SpiFlash *spi_flash,int width,int height,uint32_t data_addr)
	{
		this->spi_flash=spi_flash;
		set_size(width,height);
		this->data_addr=data_addr;
	}
	uint8_t* read_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
		spi_flash->read_random(data_addr+bbw*y+data_offset,data,data_len);
		return data;
	}
	void write_line_data(int y,uint8_t *data,int data_len,int data_offset=0)
	{
	}
};

#endif

