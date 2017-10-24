#include "CmdBase.h"
#include "Textout.h"

CMD_0x10(0x04)
{
	if(CmdLen < 3)return CMD_CONTINUE;
	tx("\x12",1);
	return CMD_FINISH;
}

