#include <mbed.h>
#include "Comm.h"
#include "CmdBase.h"
#include "board.h"
#include "FifoRam.hpp"

static void comm_cmd_handler(const void *p)
{
	Timer timer;

	uint8_t ch;

	while(1)
	{
		while(comm->fifo_rx->getch(&ch))
		{
			cmd_putc(ch);
			timer.reset();
		}
		if(timer.read_ms()>3000)
		{
			//cmd_flush();
		}
		wait_ms(1);
	}
}


static void init_comm_cmd_handler(void)
{
	new Thread("Cmd Handler",comm_cmd_handler,0,osPriorityLow,1024);
}

INIT_CALL("4",init_comm_cmd_handler);

