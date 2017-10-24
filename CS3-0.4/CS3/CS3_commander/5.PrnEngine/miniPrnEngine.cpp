#include <mbed.h>
#include "Board.h"
#include "FifoRam.hpp"
#include "PrnEngine.h"

#define FIFO_RX_PRN_SIZE	(4*1024)

PrnEngine *prnEngine;
Fifo *fifo_rx_prn;
REG_BUFFER(FIFO_RX_PRN,fifo_rx_prn,"TX");

static void init_prn_engine(void)
{
	prnEngine=new PrnEngine;
}
INIT_CALL("5",init_prn_engine);

PrnEngine::PrnEngine()
{
	fifo_rx=new FifoRam(board->ram,FIFO_RX_PRN_SIZE,"FIFO_RX_PRN");
//	fifo_rx=new Fifo(FIFO_RX_PRN_SIZE);
	fifo_rx_prn=fifo_rx;
}

