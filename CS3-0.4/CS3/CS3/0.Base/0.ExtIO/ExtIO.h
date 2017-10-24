#ifndef EXT_IO_H
#define EXT_IO_H

#include <mbed.h>

class ExtIO
{
public:
	SPI	spi;
public:
	union
	{
		struct 
		{
			uint8_t b0:1;
			uint8_t b1:1;
			uint8_t b2:1;
			uint8_t b3:1;
			uint8_t b4:1;
			uint8_t b5:1;
			uint8_t b6:1;
			uint8_t b7:1;
		}bit;
		uint8_t data;
	}OUT;
	union
	{
		struct 
		{
			uint8_t b0:1;
			uint8_t b1:1;
			uint8_t b2:1;
			uint8_t b3:1;
			uint8_t b4:1;
			uint8_t b5:1;
			uint8_t b6:1;
			uint8_t b7:1;
		}bit;
		uint8_t data;
	}IN;
public:
	ExtIO(PinName dout,PinName din,PinName sclk,PinName ncs);
	void update(void);
};
extern ExtIO *extIo;

extern int extIo_update_disable;
extern void extIo_update(void);

extern int ext_io_in_b0(void);
extern int ext_io_in_b1(void);
extern int ext_io_in_b2(void);
extern int ext_io_in_b3(void);
extern int ext_io_in_b4(void);
extern int ext_io_in_b5(void);
extern int ext_io_in_b6(void);
extern int ext_io_in_b7(void);

extern int ext_io_out_b0(int v);
extern int ext_io_out_b1(int v);
extern int ext_io_out_b2(int v);
extern int ext_io_out_b3(int v);
extern int ext_io_out_b4(int v);
extern int ext_io_out_b5(int v);
extern int ext_io_out_b6(int v);
extern int ext_io_out_b7(int v);

#define EXT_IN(bit)		((PinName)((uint32_t)ext_io_in_b##bit))
#define EXT_OUT(bit)	((PinName)((uint32_t)ext_io_out_b##bit))

#endif

