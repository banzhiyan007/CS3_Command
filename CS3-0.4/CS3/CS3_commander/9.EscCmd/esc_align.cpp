#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x61)		//ESC 'a'	选择对齐方式
{
	if(CmdLen < 3)return CMD_CONTINUE;
	if(textout->cur_x!=textout->margin_left)return CMD_FINISH;
	if(Cmd[2]&(~0x33)) return CMD_FINISH;
	textout->align = (Cmd[2]%0x30)%3;
	return CMD_FINISH;
}

