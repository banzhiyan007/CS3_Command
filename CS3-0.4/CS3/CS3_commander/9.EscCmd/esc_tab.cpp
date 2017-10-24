#include "CmdBase.h"
#include "Textout.h"

#define TAB_POS_MAX 32

static int TabPosIndex;
static int TabPosCount;
static int TabPos_Array[TAB_POS_MAX];

static void esc_reset(void)
{
	TabPosIndex = 0;
	for(int i=0;i<TAB_POS_MAX;i++)
	{
		if(i<7)TabPos_Array[i] = (i)*(canvas->Width/6) ;
		else TabPos_Array[i] = 0;
	}
	TabPosCount = 6;
}

static void init(void)
{
	esc_reset();
	TEXTOUT_RESET_FUNC_ADD(esc_reset);
}
static INIT_CALL("9",init);

CMD_0x1B(0x44)	//11. ESC 'D'		设置水平制表位
{
	//与ULT113的手册有所变化,超过8个的数据被忽略,而不是被处理为普通数据
	//每个tab数值单位为英文宽度,包括字符间距
	if(CmdLen==3)
	{
		TabPosIndex = 1;
		TabPosCount = 1;
		int i;
		for(i=0;i<TAB_POS_MAX;i++)TabPos_Array[i] = 0;
	}
	if((Cmd[CmdLen-1]==0)||(Cmd[CmdLen-1]==0xff)) return CMD_FINISH;
	int newTabPos = Cmd[CmdLen-1]*textout->default_char_width;
	TabPos_Array[TabPosIndex]=newTabPos;
	TabPosIndex++;
	TabPosCount++;

	return CMD_CONTINUE;
}

CMD(0x09)		//12. HT			移动打印位置到下一个水平制表位置
{
	int curentCurX = textout->cur_x;
	int i;
	for(i=0;i<TabPosCount;i++)
	{
		if(curentCurX<TabPos_Array[i])
		{
			textout->cur_x = TabPos_Array[i];
			break;
		}
	}
	return CMD_FINISH;
}

