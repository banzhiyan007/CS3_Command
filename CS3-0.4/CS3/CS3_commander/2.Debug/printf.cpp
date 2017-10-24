#include <stdio.h>
#include <stdarg.h>
#include <mbed.h>

extern Fifo *fifo_tx_debug;

int printf(const char *fmt,...)
{
	char buf[80];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf,fmt,args);
	va_end(args);
	int i=0;
	int len=strlen(buf);
	while(fifo_tx_debug->free_length()<len)wait_ms(2);
	fifo_tx_debug->write(buf,len);

	return i;
}

void debug_write(uint8_t *buf,int len)
{
	while(fifo_tx_debug->free_length()<len)wait_ms(2);
	fifo_tx_debug->write(buf,len);
}