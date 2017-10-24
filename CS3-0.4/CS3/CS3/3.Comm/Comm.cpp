#include <mbed.h>
#include "comm.h"

#define FIFO_RX_COMM_SIZE		1024
#define FIFO_TX_COMM_SIZE		512

COMM *comm;

Fifo *fifo_tx_comm;
Fifo *fifo_rx_comm;
REG_BUFFER(FIFO_TX_COMM,fifo_tx_comm,"RX");
REG_BUFFER(FIFO_RX_COMM,fifo_rx_comm,"TX");
VAR_REG_U32(OS_INFO_SEND,OS_INFO_SEND,0,"OS_INFO_SEND");

extern Thread *os_info_thread;
extern void (*os_info_task_write)(char *,int);
extern int os_info_send_info;
static void thread_comm(const void *p)
{
	uint8_t data[128];
	int len;
	while(1)
	{
		if(os_info_send_info)
		{
			os_info(os_info_task_write);
			os_info_send_info=0;
			OS_INFO_SEND=1;
		}
		if(comm->active_channel!=COMM::CHANNEL_NULL)
		{
			if(comm->active_timer.read_ms()>3000)
			{
				comm->active_channel=COMM::CHANNEL_NULL;
				fifo_tx_comm=comm->fifo_tx;
			}
		}

		if(comm->active_channel!=COMM::CHANNEL_NULL)
		{
			while((len=comm->fifo_tx->read(data,sizeof(data)))>0)
			{
				comm->tx(data,len);
			}
		}
		wait_ms(1);
	}
}

COMM::COMM()
{
	tx=NULL;
	fifo_rx=new Fifo(FIFO_RX_COMM_SIZE);
	fifo_tx=new Fifo(FIFO_TX_COMM_SIZE);

	fifo_rx_comm=fifo_rx;
	fifo_tx_comm=fifo_tx;

	os_info_thread=new Thread("Comm Tx",thread_comm,NULL,osPriorityLow,768);
}

static void init_comm(void)
{
	comm=new COMM;
}
INIT_CALL("3",init_comm);

