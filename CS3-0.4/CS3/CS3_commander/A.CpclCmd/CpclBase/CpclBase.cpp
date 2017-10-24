#include "CpclBase.h"
#include "Comm.h"
#include "FifoRam.hpp"

static CpclParam _cpcl_param;
CpclParam *cpcl_param=&_cpcl_param;

#define PARSER_BUF_MAX	512
static char *ParserBuf;
static int ParserLen;
#define PARSER_PARAM_MAX	16
static char *ParserParams[PARSER_PARAM_MAX];
static int ParserParamCount;
static int ParserParamLen;
static int (*ActiveParserHandler)(char *Params[],int ParamCount,char ch,int (*param_parser)(char ch),void (*tx)(char *buf,int len));
CPCL_CMD_NODE *cpcl_cmd_node=NULL;

static void parser_reset(void);
static void init(void)
{
	ParserBuf=(char*)malloc(PARSER_BUF_MAX);
	parser_reset();
	CMD_RESET_FUNC_ADD(parser_reset);
}
static INIT_CALL("8",init);

void cpcl_cmd_node_set_ext_name(CPCL_CMD_NODE *node,const char *name1,const char *name2,const char *name3)
{
	node->name1=name1;
	node->name2=name2;
	node->name3=name3;
}

static void parser_reset(void)
{
	((FifoRam*)comm->fifo_rx)->readed_data_hold(0);
	cpcl_param->started=0;
	cpcl_param->printing=0;
	ActiveParserHandler=NULL;
	ParserLen=0;
	ParserParamCount=0;
	ParserParamLen=0;
	ParserParams[0]=ParserBuf;
}

static int cpcl_param_parser(char ch)
{
	if(ch==0x0D)
	{
		printf("此指令没有!\r\n");	//	自己
		cpcl_param->printing;		//
		return 0;
	}
	if(ch==0x0A||ch==' ')
	{
		if(ParserParamLen==0)return 0;
		if(ParserParamCount<PARSER_PARAM_MAX)
		{
			ParserBuf[ParserLen]=0;
			ParserParams[ParserParamCount+1]=&ParserBuf[ParserLen+1];
			ParserLen++;
			ParserParamLen=0;
			ParserParamCount++;
			return ParserParamCount;
		}
		return -1;
	}
	if(ParserLen>=PARSER_BUF_MAX-4)return -1;
	ParserBuf[ParserLen++]=ch;
	ParserParamLen++;
	return 0;
}

static bool is_num(const char *str)
{
	int i=0;
	char c;
	while((c=str[i++]))
	{
		if(c<'0'||c>'9')return 0;
	}
	return 1;
}

static int strcasecmp(const char *s1, const char *s2)
{
	#define __tolower(c)    ((('A' <= (c))&&((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))
	const unsigned char *p1 = (const unsigned char *) s1;
	const unsigned char *p2 = (const unsigned char *) s2;
	int result = 0;

	if (p1 == p2)
	{
		return 0;
	}

	while ((result = __tolower(*p1) - __tolower(*p2)) == 0)
	{
		if (*p1++ == '\0')
		{
			break;
		}
		p2++;
	}

	return result;
}

void cpcl_cmd_reset(void)
{
	ParserLen=0;
	ParserParamLen=0;
	ParserParamCount=0;
	ActiveParserHandler=NULL;
}

__attribute__((weak)) void cpcl_new_page(void (*tx)(char *buf,int len))
{
}

int cpcl_handler(char ch,void (*tx)(char *buf,int len))
{
	if(!cpcl_param->started)
	{
		int n=cpcl_param_parser(ch);
		if(n<0)return CMD_MISS_MATCH;
		if(n==0)return CMD_CONTINUE;
		for(int i=1;i<n;i++)
		{
			if(!is_num(ParserParams[i]))return CMD_MISS_MATCH;
		}
		if(n<6&&(ch==0x0D||ch==0x0A))return CMD_MISS_MATCH;
		if(n==6)
		{
			if(ch!=0x0D&&ch!=0x0A)return CMD_MISS_MATCH;
			cpcl_param->offset=atol(ParserParams[1]);
			cpcl_param->resolutionX=atol(ParserParams[2]);
			cpcl_param->resolutionY=atol(ParserParams[3]);
			cpcl_param->height=atol(ParserParams[4]);
			cpcl_param->quantity=atol(ParserParams[5]);
			cpcl_new_page(tx);
			cpcl_param->started=1;
			((FifoRam*)comm->fifo_rx)->readed_data_hold(1);
			cpcl_cmd_reset();
		}
		return CMD_CONTINUE;
	}
	else
	{
		if(ActiveParserHandler==NULL)
		{
			int n=cpcl_param_parser(ch);
			if(n<0)return CMD_MISS_MATCH;
			if(n==0)return CMD_CONTINUE;
			CPCL_CMD_NODE *node=cpcl_cmd_node;
			while(node)
			{
				if(strcasecmp(node->name,ParserParams[0])==0
					||strcasecmp(node->name1,ParserParams[0])==0
					||strcasecmp(node->name2,ParserParams[0])==0
					||strcasecmp(node->name3,ParserParams[0])==0)
				{
					ActiveParserHandler=node->handler;
					int r=ActiveParserHandler(ParserParams,ParserParamLen,ch,cpcl_param_parser,tx);
					if(r==CMD_FINISH)cpcl_cmd_reset();
					if(cpcl_param->started==0)return CMD_FINISH;
					return CMD_CONTINUE;
				}
				node=node->pNext;
			}
			cpcl_cmd_reset();
			return CMD_CONTINUE;
		}
		else
		{
			int r=ActiveParserHandler(ParserParams,ParserParamLen,ch,cpcl_param_parser,tx);
			if(r==CMD_FINISH)cpcl_cmd_reset();
			if(cpcl_param->started==0)return CMD_FINISH;
			return CMD_CONTINUE;
		}
	}
	return CMD_CONTINUE;
}

CMD(0x21)	//!
{
	uint8_t ch=Cmd[CmdLen-1];
	(*pCmdLen)--;
	return cpcl_handler(ch,tx);
}

