#include "CmdBase.h"
#include "Textout.h"

CMD_0x1D(0x50)		//GS 'P'		���ú���������ƶ���λ
{
	if(CmdLen<4) return CMD_CONTINUE;
//	cmd_print.UnitDpiX = Cmd[2]?Cmd[2]:203;
//	cmd_print.UnitDpiY = Cmd[3]?Cmd[3]:203;
	return CMD_FINISH;
}