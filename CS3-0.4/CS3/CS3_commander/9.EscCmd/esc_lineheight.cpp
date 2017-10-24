#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x32)		//ESC '2'	设置默认行高
{
	textout->line_height = 30;
	return CMD_FINISH;
}

CMD_0x1B(0x33)		//ESC '3'	设置行高
{
	if(CmdLen<3) return CMD_CONTINUE;
	int lineHeight = Cmd[2];
	if(lineHeight>1016*8)lineHeight = 1016*8;
	textout->line_height = lineHeight;
	return CMD_FINISH;
}

