#include "CpclBase.h"
#include "Comm.h"
#include "FifoRam.hpp"

int cpcl_handler(char ch,void (*tx)(char *buf,int len));
static void cpcl_redo_cmd(void (*tx)(char *buf,int len))
{
	char buf[128];
	int pos=0;
	int total_len=((FifoRam*)comm->fifo_rx)->readed_data_len();
	while(pos<total_len)
	{
		int len=total_len-pos;
		if(len>sizeof(buf))len=sizeof(buf);
		((FifoRam*)comm->fifo_rx)->readed_data_get(pos,buf,len);
		for(int i=0;i<len;i++)cpcl_handler(buf[i],tx);
		pos+=len;
	}
}

static void cpcl_print_page(void (*tx)(char *buf,int len));
CPCL_CMD(PRINT)
{
	if(ch!=0x0A)return CMD_CONTINUE;
	if(cpcl_param->printing)return CMD_FINISH;
	cpcl_param->printing=1;
	cpcl_print_page(tx);
	for(int i=1;i<cpcl_param->quantity;i++)
	{
		cpcl_cmd_reset();
		cpcl_new_page(tx);
		cpcl_redo_cmd(tx);
		cpcl_print_page(tx);
	}
	cpcl_param->printing=0;
	((FifoRam*)comm->fifo_rx)->readed_data_hold(0);
	cpcl_param->started=0;
	return CMD_FINISH;
}

static void cpcl_print_page(void (*tx)(char *buf,int len))
{
	tx("cpcl_cmd_print\n",strlen("cpcl_cmd_print\n"));
}

