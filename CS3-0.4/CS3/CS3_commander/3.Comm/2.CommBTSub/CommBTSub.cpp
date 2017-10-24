#include <mbed.h>
#include "version.h"
#include "board.h"
#include "LinkReg.h"
#include "comm.h"
#include "CommBTSub.h"

extern Fifo *fifo_tx_comm;
extern BT_STATE bt_state;
REG_U32(BT_SUB_STATE,&bt_state,"¸±À¶ÑÀ×´Ì¬");

RawBluetooth	*comm_bt;

static void bt_tx(uint8_t *data,int len)
{
	comm_bt->write(data,len);
}

static void on_bt_rx(uint8_t *data,int len)
{
	comm->active_channel=COMM::CHANNEL_BT;
	fifo_tx_comm=NULL;
	comm->tx=bt_tx;
	comm->active_timer.reset();
	while(!comm->fifo_rx->write(data,len))wait_ms(1);
}

static int bt_wait_init(void)
{
	if(regRemoteCount==0)return 1;
	comm_bt->BtName=link_reg_get_string("BLUETOOTH_NAME","");
	comm_bt->BtPass=link_reg_get_string("BLUETOOTH_PASSWORD","");
	static uint8_t addr[6];
	memcpy(addr,link_reg_get_bytes("BLUETOOTH_ADDR",(uint8_t*)""),6);
	comm_bt->BtAddr=addr;
	return 0;
}

class UI_Bluetooth
{
public:
	UI_Bluetooth()
	{
		comm_bt=new RawBluetooth("Printer",(uint8_t*)"\x00\x00\x00\x00\x00","0000",BT_MODE_BLE);//BT_MODE_BT);//BT_MODE_BLE);
		comm_bt->wait_init_func=bt_wait_init;
		//comm_bt->
		free(comm_bt->fifo_rx->Begin);
		delete comm_bt->fifo_rx;
		comm_bt->fifo_rx=comm->fifo_rx;
		comm_bt->on_rx=on_bt_rx;
	}
	void bt_enabled_changed(void)
	{
	}
};

static void init_bt(void)
{
	new UI_Bluetooth;
}
INIT_CALL("4",init_bt);

