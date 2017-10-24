#include <mbed.h>

#define FIFO_TX_DEBUG_SIZE	512

Fifo *fifo_tx_debug;
REG_BUFFER(FIFO_TX_DEBUG,fifo_tx_debug,"TX");

/*
static void task_debug(const void *p)
{
	while(1)
	{
		wait_ms(1000);
		printf("task_debug\n");
	}
}
*/

static void init_debug(void)
{
	fifo_tx_debug=new Fifo(FIFO_TX_DEBUG_SIZE);
//	new Thread("debug",task_debug,NULL,osPriorityLow,768);
}

INIT_CALL("2",init_debug);


