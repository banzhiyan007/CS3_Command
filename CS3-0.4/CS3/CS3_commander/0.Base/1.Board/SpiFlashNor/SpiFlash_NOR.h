#ifndef SPI_FLASH_NOR_H
#define SPI_FLASH_NOR_H

#include <SpiFlash.h>

class SpiFlash_NOR:public SpiFlash
{
private:
	SPI _spi;
	DigitalOut _ncs;
	uint32_t page_size;
public:
	SpiFlash_NOR(PinName mosi, PinName miso, PinName sclk, PinName cs);
	virtual int status();
	virtual int read_random(uint32_t addr,uint8_t * buffer, uint32_t len);
	virtual int read_sector(uint8_t * buffer, uint32_t sector_number){}
	virtual int read_block(uint8_t *buffer,uint32_t block_number);
	virtual int write_sector(const uint8_t * buffer, uint32_t sector_number){}
	virtual int write_block(const uint8_t *buffer, uint32_t block_number);
	virtual void cache_update(){}
	virtual uint32_t sectors(){}
private:
	void _write_enable(bool enabled);
	void _erase_block(uint32_t block_index);
	void _write_page(uint32_t page_index,const uint8_t *data);
	void _busy(void);
	void get_device_id(void);
};

#endif

