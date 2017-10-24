#ifndef COMM_H
#define COMM_H

class COMM
{
public:
	COMM();
	enum
	{
		CHANNEL_NULL,
		CHANNEL_SERIAL,
		CHANNEL_USB,
		CHANNEL_BT,
	}active_channel;
	void (*tx)(uint8_t *data,int len);
	Timer active_timer;
	Fifo *fifo_rx;
	Fifo *fifo_tx;
};
extern COMM *comm;

#endif

