#ifndef USB_PRN_H
#define USB_PRN_H

#include <USBPrinter.h>

class CommUsb
{
public:
	RawUSBPrinter	*usb;
public:
	CommUsb();
};

extern CommUsb *commUsb;

#endif

