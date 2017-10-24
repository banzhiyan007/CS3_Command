#include <mbed.h>
#include "board.h"
#include "comm.h"
#include "CommUsb.h"

CommUsb *commUsb=new CommUsb;

void usb_tx(uint8_t *data,int len)
{
	commUsb->usb->write(data,len);
}

static void on_usb_rx(uint8_t *data,int len)
{
	comm->active_channel=COMM::CHANNEL_USB;
	comm->tx=usb_tx;
	comm->active_timer.reset();
	while(!comm->fifo_rx->write(data,len))wait_ms(1);
}

CommUsb::CommUsb()
{
	usb = new RawUSBPrinter(reg_get_string("OEM_MANUFACTURE"),reg_get_string("OEM_MODELNAME"));
	usb->disconnect();
	delete usb->fifo_rx;
	usb->fifo_rx=comm->fifo_rx;
	usb->on_rx=on_usb_rx;
}

