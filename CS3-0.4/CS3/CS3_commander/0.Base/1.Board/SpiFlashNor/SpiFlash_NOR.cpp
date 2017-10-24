#include "mbed.h"
#include "SpiFlash_NOR.h"

SpiFlash_NOR::SpiFlash_NOR(PinName mosi, PinName miso, PinName sclk, PinName cs):_spi(mosi,miso,sclk),_ncs(cs)
{
	locked=false;
	_spi.format(SPIFirstBitMSB,8,0);
	_spi.frequency(104000000);

	block_size = 0;
	block_count = 0;
	reserved = isp_spi_flash_reserved();
	get_device_id();
	get_device_id();
}

int SpiFlash_NOR::status()
{
	return 0;
}

int SpiFlash_NOR::read_random(uint32_t addr,uint8_t *buffer, uint32_t len)
{
	if (len==0)return 0;
	while(locked)wait_ms(1);
	locked=true;
	addr+=reserved;
	uint8_t d[4];
	d[0]=0x03;
	d[1]=(addr>>16)&0xFF;
	d[2]=(addr>>8)&0xFF;
	d[3]=(addr>>0)&0xFF;
	_busy();
	_ncs = 0;
	_spi.write((char*)d,4);
	_spi.read((char*)buffer,len);
	_ncs = 1;
	locked=false;
	return 0;
}

int SpiFlash_NOR::read_block(uint8_t *buffer,uint32_t block_number)
{
	uint32_t addr=block_number*block_size;
	read_random(addr,buffer,block_size);
	return 0;
}

int SpiFlash_NOR::write_block(const uint8_t *buffer, uint32_t block_number)
{
	while(locked)wait_ms(1);
	locked=true;
	uint32_t page_base=block_number*block_size/page_size;
	uint32_t page;
	_write_enable(true);
	_erase_block(block_number);
	_write_enable(false);
	_busy();
	for(page=0;page<block_size/page_size;page++)
	{
		_write_enable(true);
		_write_page(page_base+page,buffer+page*page_size);
		_write_enable(false);
		_busy();
	}
	_write_enable(false);
	locked=false;
	return 0;
}

void SpiFlash_NOR::_write_enable(bool enabled)
{
	_ncs = 0;
	_spi.write(enabled?0x06:0x04);
	_ncs = 1;
}

void SpiFlash_NOR::_erase_block(uint32_t block_index)
{
	uint32_t addr=block_index*block_size+reserved;
	_ncs = 0;
	uint8_t d[4];
	d[0]=0x20;
	d[1]=(addr>>16)&0xFF;
	d[2]=(addr>>8)&0xFF;
	d[3]=(addr>>0)&0xFF;
	for(uint32_t i=0;i<4;i++)_spi.write(d[i]);
	_ncs = 1;
}

void SpiFlash_NOR::_write_page(uint32_t page_index,const uint8_t *data)
{
	uint32_t addr=page_index*page_size+reserved;
	uint8_t d[4];
	d[0]=0x02;
	d[1]=(addr>>16)&0xFF;
	d[2]=(addr>>8)&0xFF;
	d[3]=(addr>>0)&0xFF;
	_ncs = 0;
	_spi.write((char*)d,4);
	_spi.write((char*)data,page_size);
	_ncs = 1;
}

void SpiFlash_NOR::_busy(void)
{
	volatile uint8_t IsBusy = 1;
	while (IsBusy)
	{
		_ncs = 0;
		_spi.write(0x05);
		IsBusy = ((_spi.write(0x00)) & 0x01);
		_ncs = 1;
	}
}

void SpiFlash_NOR::get_device_id(void)
{
	_ncs = 0;
	_spi.write(0x9f);
	uint8_t ManufactureID = _spi.write(0x00);
	uint8_t MemoryType = _spi.write(0x00);
	uint8_t Capacity = _spi.write(0x00);
	_ncs = 1;
	ManufactureID = ManufactureID;
	if(/*ManufactureID==0xEF&&*/MemoryType==0x40)
	{
		switch(Capacity)
		{
		case 0x16:
			block_size=4096;
			page_size=256;
			block_count=1024;
			break;
		case 0x17:
			block_size=4096;
			page_size=256;
			block_count=2048;
			break;
		case 0x18:
			block_size=4096;
			page_size=256;
			block_count=4096;
			break;
		case 0x19:
			block_size=4096;
			page_size=256;
			block_count=8192;
			break;
		}
	}
	if(block_size)block_count-=reserved/block_size;
	//  printf("Mfc:%02X Mtype:%02X Cap:%02X\r\n",ManufactureID,MemoryType,Capacity);
	//  printf("block_size:%ld page_size:%ld block_count:%ld\r\n",block_size,page_size,block_count);
	//	while(1);
}
