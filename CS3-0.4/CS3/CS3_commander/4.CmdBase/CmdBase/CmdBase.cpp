#include <mbed.h>
#include "Comm.h"
#include "cmd_table.hpp"
#include "CmdBase.h"

#define CMD_LEN_MAX	512

static Timer    *CmdTimer;
static uint8_t  *CmdBuf;
static int   CmdLen;
static int (*ActiveHandler)(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len));
__attribute__((weak)) void cmd_textout(char ch){}
CMD_RESET_FUNC_LIST_NODE *cmd_reset_func_node=NULL;

static void init_cmd(void)
{
	CmdTimer=new Timer;
	CmdBuf=(uint8_t*)malloc(CMD_LEN_MAX);
}
static INIT_CALL("4",init_cmd);

void cmd_tx(char *buf,int len)
{
	while(comm->fifo_tx->free_length()<len)wait_ms(1);
	comm->fifo_tx->write(buf,len);
}

static void cmd_reset(void)
{
	ActiveHandler=NULL;
	CmdLen=0;
	CMD_RESET_FUNC_LIST_NODE *node=cmd_reset_func_node;
	while(node!=NULL)
	{
		node->func();
		node=node->pNext;
	}
}

void cmd_flush(void)
{
	for(int i=0;i<CmdLen;i++)cmd_textout(CmdBuf[i]);
	cmd_reset();
}

void cmd_putc(char c)
{
	if(CmdTimer->read_ms()>3000)cmd_reset();
	if(ActiveHandler)
	{
		if(CmdLen<CMD_LEN_MAX)CmdBuf[CmdLen++]=c;
		int r=ActiveHandler(CmdBuf,CmdLen,&CmdLen,cmd_tx);
		if(r==CMD_FINISH)cmd_reset();
		else if(r==CMD_MISS_MATCH)cmd_flush();
	}
	else
	{
		if(gCmdHandler_Array[(uint8_t)c]!=NULL)
		{
			if(CmdLen<CMD_LEN_MAX)CmdBuf[CmdLen++]=c;
			int r=gCmdHandler_Array[(uint8_t)c](CmdBuf,CmdLen,&CmdLen,cmd_tx);
			if(r==CMD_CONTINUE)ActiveHandler = gCmdHandler_Array[(uint8_t)c];
			else if(r==CMD_MISS_MATCH)cmd_flush();
			else cmd_reset();
		}
		else
		{
			cmd_textout(c);
		}
	}
	CmdTimer->reset();
}

//-------------------------------------------------------------------------------------
CMD(0x00)
{
	return CMD_FINISH;
}
//-------------------------------------------------------------------------------------
CMD(0x10)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x10_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
//-------------------------------------------------------------------------------------
CMD(0x1B)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x1b_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
//-------------------------------------------------------------------------------------
CMD(0x1C)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x1c_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
//-------------------------------------------------------------------------------------
CMD(0x1D)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x1d_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
//-------------------------------------------------------------------------------------
CMD(0x1E)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x1e_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
//-------------------------------------------------------------------------------------
CMD(0x1F)
{
	if(CmdLen==2)
	{
		ActiveHandler = gCmdHandler0x1f_Array[(uint8_t)Cmd[1]];
		if(ActiveHandler!=NULL){return ActiveHandler(Cmd,CmdLen,&CmdLen,tx);}
		return CMD_FINISH;
	}
	return CMD_CONTINUE;
}
