#include <mbed.h>

#define FIFO_TX_DEBUG_SIZE	512

static Fifo *fifo_tx_debug;
REG_BUFFER(FIFO_TX_DEBUG,fifo_tx_debug,"RX");

static void task_debug(const void *p)
{
	char buf[128];
	while(1)
	{
		int len;
		while(len=fifo_tx_debug->read(buf,sizeof(buf)))
		{
			extern void debug_serial_write(char ch);
			for(int i=0;i<len;i++)debug_serial_write(buf[i]);
		}
		wait_ms(1);

		//调试
		//printf("进入调试1!\n");
		//wait_ms(500);
	}
}

static void init_debug(void)
{
	fifo_tx_debug=new Fifo(FIFO_TX_DEBUG_SIZE);
	new Thread("debug",task_debug,NULL,osPriorityLow,512);
}

INIT_CALL("9",init_debug);


