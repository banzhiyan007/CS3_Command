#ifndef CMD_BASE_H
#define CMD_BASE_H

#include <mbed.h>

#define CMD_CONTINUE		1
#define CMD_FINISH			2
#define CMD_MISS_MATCH		3

#define CMD(code)		int cmd_handler_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x10(code)	int cmd_handler_0x10_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x1B(code)	int cmd_handler_0x1b_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x1C(code)	int cmd_handler_0x1c_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x1D(code)	int cmd_handler_0x1d_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x1E(code)	int cmd_handler_0x1e_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_0x1F(code)	int cmd_handler_0x1f_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))
#define CMD_ESC(code)	int cmd_handler_0x1b_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))

struct CMD_RESET_FUNC_LIST_NODE
{
	void (*func)(void);
	CMD_RESET_FUNC_LIST_NODE *pNext;
};
extern CMD_RESET_FUNC_LIST_NODE *cmd_reset_func_node;
#define CMD_RESET_FUNC_ADD(f)	{static CMD_RESET_FUNC_LIST_NODE node={f,NULL};node.pNext=cmd_reset_func_node;cmd_reset_func_node=&node;}

extern void cmd_putc(char c);
extern void cmd_flush(void);

#endif

