#include "CmdBase.h"
#include "Textout.h"

CMD_0x1D(0x4C)		//GS 'L'	                设置左边距
{
	if(CmdLen <4 ) return CMD_CONTINUE;
	uint32_t v = Cmd[2]+Cmd[3]*256;
	if(v>canvas->Width)return CMD_FINISH;
	textout->margin_left = v;
	textout->cur_x=textout->margin_left;
	return CMD_FINISH;
}

CMD_0x1D(0x57)      //GS 'W'            设置打印区域宽度
{
	if(CmdLen<4)return CMD_CONTINUE;
	if(textout->cur_x!=textout->margin_left)return CMD_FINISH;
	uint16_t width=Cmd[3]*256+Cmd[2];
	uint32_t right = textout->margin_left+width-1;
	if(right > canvas->Width) right = canvas->Width;
	textout->margin_right = right;
	return CMD_FINISH;
}

