#include <mbed.h>
#include "version.h"
#include "board.h"
#include "comm.h"
#include "UiLcd.h"
#include "CommBTComb.h"
#include <Flag.hpp>

VAR_REG_STRING(BLUETOOTH_NAME,32,BluetoothName,"Printer","À¶ÑÀÃû³Æ");
VAR_REG_BYTES(BLUETOOTH_ADDR,6,BluetoothAddr,"\x00\x00\x00\x00\x00","À¶ÑÀµØÖ·");
VAR_REG_STRING(BLUETOOTH_PASSWORD,32,BluetoothPassword,"0000","À¶ÑÀÃÜÂë");
VAR_REG_U32(BT_SUB_STATE,BT_SUB_STATE,0,"¸±À¶ÑÀ×´Ì¬");

RawBluetooth	*comm_bt;
int		 bt_sub_exist=1;

static void bt_tx(uint8_t *data,int len)
{
	comm_bt->write(data,len);
}

static void on_bt_rx(uint8_t *data,int len)
{
#if 0
	void debug_serial_write(char ch);
	for(int i=0;i<len;i++)debug_serial_write(data[i]);
	return;
#endif
	comm->active_channel=COMM::CHANNEL_BT;
	comm->tx=bt_tx;
	comm->active_timer.reset();
	while(!comm->fifo_rx->write(data,len))wait_ms(1);
}

class UI_Bluetooth
{
private:
	Icon icon_bluetooth;
	Icon icon_online;
	Icon icon_connected;
	Icon icon_disabled;
private:
	char STR_BT_INFO[16+1];
	char STR_BT_ENABLE[16+1];
	void on_language_change(int language_id)
	{
		if(language_id==0)
		{
			strcpy(STR_BT_INFO,"[BT INFO]");
			sprintf(STR_BT_ENABLE,"BT ENABLE:%lu",reg_get_u32("BT_ENABLED"));
		}
		else if(language_id==1)
		{
			strcpy(STR_BT_INFO,"[À¶ÑÀÐÅÏ¢]");
			sprintf(STR_BT_ENABLE,"À¶ÑÀ¿ª¹Ø:%lu",reg_get_u32("BT_ENABLED"));
		}
	}

	Flag flag_state;
	Flag sub0_flag_state;
	void bluetooth_status_handler(int redraw)
	{
		BT_STATE sub0_bt_state=BT_STATE_INIT;
		if(bt_sub_exist==0)sub0_bt_state=(*comm_bt->state);
		else sub0_bt_state=(BT_STATE)reg_get_u32("BT_SUB_STATE",0);

		flag_state.set(*comm_bt->state);
		sub0_flag_state.set(sub0_bt_state);

		if(*comm_bt->state==BT_STATE_CONNECTED||sub0_bt_state==BT_STATE_CONNECTED)sleep_clear();

		if(*comm_bt->state==BT_STATE_DISABLED&&sub0_bt_state==BT_STATE_DISABLED)
		{
			if(redraw)
			{
				uiLcd->lcd->display_icon(&icon_disabled);
			}
		}
		else if(*comm_bt->state<BT_STATE_ONLINE||sub0_bt_state<BT_STATE_ONLINE)
		{
			static Flag *flash=NULL;
			if(flash==NULL)flash=new Flag(300);
			if(flash->changed())
			{
				if((*comm_bt->state==BT_STATE_IDLE||*comm_bt->state==BT_STATE_INIT)
					||(sub0_bt_state==BT_STATE_IDLE||sub0_bt_state==BT_STATE_INIT))
				{
					uiLcd->lcd->display_icon(&icon_bluetooth,!flash->status());
				}
				if((*comm_bt->state==BT_STATE_CONFIG||*comm_bt->state==BT_STATE_OFFLINE)
					||(sub0_bt_state==BT_STATE_CONFIG||sub0_bt_state==BT_STATE_OFFLINE))
				{
					uiLcd->lcd->display_icon(&icon_bluetooth,!flash->status());
				}
			}
		}
		else
		{
			if(flag_state.changed()||sub0_flag_state.changed()||redraw)
			{
				if(*comm_bt->state==BT_STATE_ONLINE&&sub0_bt_state==BT_STATE_ONLINE)
				{
					uiLcd->lcd->display_icon(&icon_online);
					board->lcd->printf12(26,-1,"%02X:%02X",comm_bt->BtAddr[4],comm_bt->BtAddr[5]);
				}
				if(*comm_bt->state==BT_STATE_CONNECTED||sub0_bt_state==BT_STATE_CONNECTED)
				{
					uiLcd->lcd->display_icon(&icon_connected);
				}
			}
		}
	}
	void bt_info(void)
	{
		uiLcd->lcd->printf12(0,-1,"NAME: %s",comm_bt->BtName);
		uiLcd->lcd->printf12(0,10,"ADDR: %02X%02X%02X%02X%02X%02X",comm_bt->BtAddr[0],comm_bt->BtAddr[1],comm_bt->BtAddr[2],comm_bt->BtAddr[3],comm_bt->BtAddr[4],comm_bt->BtAddr[5]);
//		board->lcd->printf12(0,21,"RX: %d TX:%d ",bluetooth->Status.RxCount,bluetooth->Status.TxCount);
	}
	void bt_info_handler(uint8_t *pPage,int page)
	{
		if(page>=1){*((uint32_t*)pPage)=page=0;}
		if(page==0)bt_info();
	}
public:
	UI_Bluetooth()
	{
		if(memcmp(BluetoothAddr,"\x00\x00\x00\x00\x00\x00",6)==0)
		{
			void bt_get_default_addr(uint8_t addr[6]);
			bt_get_default_addr(BluetoothAddr);

			sprintf(BluetoothName,"Printer%s_%02X%02X",MODEL_NAME,BluetoothAddr[4],BluetoothAddr[5]);
			strcpy(BluetoothPassword,"0000");
			//reg_value_save();
		}
		comm_bt=new RawBluetooth(BluetoothName,BluetoothAddr,BluetoothPassword,BT_MODE_BT);//BT_MODE_BT);

		free(comm_bt->fifo_rx->Begin);
		delete comm_bt->fifo_rx;
		comm_bt->fifo_rx=comm->fifo_rx;
		comm_bt->on_rx=on_bt_rx;

		icon_bluetooth={0,0,24,12,"\x00\x00\x00\x18\x00\x00\x28\x00\x00\x4A\x00\x00\x2C\x00\x00\x18\x00\x00\x2C\x00\x00\x4A\x04\x00\x28\x0E\x00\x18\x04\x00\x00\x00\x00\x00\x00\x00"};
		icon_online={0,0,24,12,"\x00\x00\x00\x18\x00\x00\x28\x00\x00\x4A\x00\x00\x2C\x00\x00\x18\x9C\x03\x2C\x24\x01\x4A\x1C\x11\x28\x24\x39\x18\x1C\x11\x00\x00\x00\x00\x00\x00"};
		icon_connected={0,0,24,12,"\x3C\x00\x00\x66\x00\x00\xD7\x00\x00\xB5\x00\x00\xD3\x00\x00\xE7\x9C\x03\xD3\x24\x01\xB5\x1C\x11\xD7\x24\x39\x66\x1C\x11\x3C\x00\x00\x00\x00\x00"};
		icon_disabled={0,0,24,12,"\x00\x00\x00\x18\x00\x00\x28\x00\x00\x4A\x20\x08\x2C\x40\x04\x18\x80\x02\x2C\x00\x01\x4A\x84\x02\x28\x4E\x04\x18\x24\x08\x00\x00\x00\x00\x00\x00"};

		on_language_change(reg_get_u32("LANGUAGE",0));
		uiLcd->add_language_handler(new FunctionPointer1Arg(this,&UI_Bluetooth::on_language_change));

		icon_bluetooth.x = uiLcd->IconX;
		icon_bluetooth.y = uiLcd->IconY;
		uiLcd->icon_malloc(icon_bluetooth.Width,icon_bluetooth.Height);
		icon_online.x=icon_bluetooth.x;
		icon_online.y=icon_bluetooth.y;
		icon_connected.x=icon_bluetooth.x;
		icon_connected.y=icon_bluetooth.y;

		uiLcd->add_status_handler(new FunctionPointer1Arg(this,&UI_Bluetooth::bluetooth_status_handler));
		uiLcd->config_add_display_info(0,STR_BT_INFO,new FunctionPointer2Arg(this,&UI_Bluetooth::bt_info_handler));
		uiLcd->config_add_function(1,STR_BT_ENABLE,new FunctionPointer(this,&UI_Bluetooth::bt_enabled_changed),false);
	}
	void bt_enabled_changed(void)
	{
		reg_set_u32("BT_ENABLED",!reg_get_u32("BT_ENABLED",1));
		//reg_value_save();
		on_language_change(reg_get_u32("LANGUAGE",0));
	}
};

static void init_bt(void)
{
	new UI_Bluetooth;
}
INIT_CALL("4",init_bt);

