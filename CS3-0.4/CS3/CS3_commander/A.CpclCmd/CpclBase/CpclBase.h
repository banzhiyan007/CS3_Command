#ifndef CPCL_BASE_H
#define CPCL_BASE_H

#include "CmdBase.h"
#include "CpclParam.h"
#include "canvas.h"

struct CPCL_CMD_NODE
{
	const char *name;
	const char *name1;
	const char *name2;
	const char *name3;
	int (*handler)(char *Params[],int ParamCount,char ch,int (*param_parser)(char ch),void (*tx)(char *buf,int len));
	CPCL_CMD_NODE *pNext;
};
extern CPCL_CMD_NODE *cpcl_cmd_node;

extern void cpcl_cmd_node_set_ext_name(CPCL_CMD_NODE *node,const char *name1="",const char *name2="",const char *name3="");

#define CPCL_CMD(name,args...)	\
	int cpcl_cmd_##name(char *Params[],int ParamCount,char ch,int (*param_parser)(char ch),void (*tx)(char *buf,int len));	\
	static void init_cpcl_cmd_##name(void){static CPCL_CMD_NODE node_##name={""#name"","","","",cpcl_cmd_##name,NULL};cpcl_cmd_node_set_ext_name(&node_##name,##args);node_##name.pNext=cpcl_cmd_node;cpcl_cmd_node=&node_##name;}	\
	INIT_CALL("9",init_cpcl_cmd_##name);	\
	int cpcl_cmd_##name(char *Params[],int ParamCount,char ch,int (*param_parser)(char ch),void (*tx)(char *buf,int len))

extern void cpcl_cmd_reset(void);
extern void cpcl_new_page(void (*tx)(char *buf,int len));

#endif

