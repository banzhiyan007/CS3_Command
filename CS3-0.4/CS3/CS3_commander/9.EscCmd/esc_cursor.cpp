#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x24)		//ESC '$'		设置绝对打印位置   x,y
{
	if(CmdLen<4) return CMD_CONTINUE;
	uint32_t v = Cmd[2]+Cmd[3]*256;
	if(v>canvas->Width) return CMD_FINISH;
	textout->cur_x = textout->margin_left+v;
	return CMD_FINISH;
}

CMD_0x1B(0x5C)		//ESC  '\'		设置水平相对打印位置
{
	if(CmdLen<4)return CMD_CONTINUE;
	int pos,v;
	v = Cmd[2]+Cmd[3]*256;
	bool dir=((Cmd[3]&0x80)==0);
	if(v>0x8000)v=(0xFFFF-v)+1;
	if(dir)	pos = textout->cur_x + v;
	else pos =  textout->cur_x - v;
	if(pos<0||pos>canvas->Width) return CMD_FINISH;
	textout->cur_x=pos;
	return CMD_FINISH;
}

