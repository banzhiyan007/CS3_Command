#include "CpclBase.h"

CPCL_CMD(TEXT,"T","T1") 
{
	tx("abc_cmd\r\n",9);
	return CMD_FINISH;
}

