#include <mbed.h>
#include "board.h"
#include "FifoRam.hpp"
#include "comm.h"

#define FIFO_RX_COMM_SIZE		(1024*1024)
#define FIFO_TX_COMM_SIZE		512

COMM *comm;

Fifo *fifo_tx_comm;
Fifo *fifo_rx_comm;
REG_BUFFER(FIFO_TX_COMM,fifo_tx_comm,"TX");
REG_BUFFER(FIFO_RX_COMM,fifo_rx_comm,"RX");
VAR_REG_U32(OS_INFO_SEND,OS_INFO_SEND,0,"OS_INFO_SEND");

extern Thread *os_info_thread;
static void os_info_write(char *data,int len)
{
	while(comm->fifo_tx->free_length()<len)wait_ms(1);
	comm->fifo_tx->write(data,len);
}
extern int os_info_send_info;
static void thread_comm(const void *p)
{
	uint8_t data[128];
	int len;
	while(1)
	{
		if(OS_INFO_SEND)
		{
			os_info(os_info_write);
			sprintf((char*)data,"SpiRam size:%dk free:%dk --------\n",board->ram->size/1024,board->ram->free_size/1024);
			os_info_write((char*)data,strlen((char*)data));

			RAM_MALLOC_INFO *malloc_info=board->ram->malloc_info;
			while(malloc_info)
			{
				sprintf((char*)data,"%08X %6X %s(%dk)\n",malloc_info->addr,malloc_info->size,malloc_info->name,malloc_info->size/1024);
				os_info_write((char*)data,strlen((char*)data));
				malloc_info=malloc_info->pNext;
			}
			OS_INFO_SEND=0;
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
	fifo_rx=new FifoRam(board->ram,FIFO_RX_COMM_SIZE,"FIFO_RX_COMM");
	fifo_tx=new Fifo(FIFO_TX_COMM_SIZE);

	fifo_rx_comm=fifo_rx;
	fifo_tx_comm=fifo_tx;

	os_info_thread=new Thread("Comm Tx",thread_comm,NULL,osPriorityLow,1024);
}

static void init_comm(void)
{
	comm=new COMM;
}
INIT_CALL("3",init_comm);

