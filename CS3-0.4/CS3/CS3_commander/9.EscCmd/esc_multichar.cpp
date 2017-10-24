#include "CmdBase.h"
#include "Textout.h"

CMD_0x1C(0x21)		//1. FS '!'		设定汉字格式
{
	if(CmdLen<3)return CMD_CONTINUE;
	textout->textStyle->fsMulti->scaleX = ((Cmd[2]&(1<<2))>0)?2:1;
	textout->textStyle->fsMulti->scaleY = ((Cmd[2]&(1<<3))>0)?2:1;
	textout->textStyle->fsMulti->underline = ((Cmd[2]&(1<<7))>0)?1:0;
	return CMD_FINISH;
}
CMD_0x1C(0x26)		//1. FS '&'		设定汉字模式
{
	textout->reset_codepage();
	return CMD_FINISH;
}
CMD_0x1C(0x2D)		//1. FS '-'		设定汉字下划线
{
	if(CmdLen<3)return CMD_CONTINUE;
	uint8_t value = (Cmd[2]%0x30)%3;
	textout->textStyle->fsMulti->underline = value;
	return CMD_FINISH;
}
CMD_0x1C(0x2E)		//2. FS '.'		取消汉字字符模式
{
	canvas->nls->set_locale(437);
	return CMD_FINISH;
}

CMD_0x1C(0x32)	// FS '2'		自定义汉字
{
	if(CmdLen<76)return CMD_CONTINUE;
	if(Cmd[2]!=0xFE)return CMD_FINISH;
	if((Cmd[3]>0xFE)||(Cmd[3]<0xA1))return CMD_FINISH;

	int index=Cmd[3]-0xA1;
	uint8_t pdata[72];
	memset(pdata,0,72);
	uint8_t pblockData[72];
	memset(pblockData,0,72);
	memcpy(pdata,&Cmd[4],72);
	int iy ,ix;
	for(iy=0;iy<8*3;iy++)
		for(ix=0;ix<24;ix++)
		{
			if(pdata[ix*3+iy/8]&(1<<(7-(iy%8))))
			{
				pblockData[3*iy+ix/8] |= (1<<(ix%8));
			}
		}
		canvas->fontUserDefine->set_char(index,24,24,pblockData);
		return CMD_FINISH;
}

CMD_0x1C(0x33)
{
	if(CmdLen<4)return CMD_CONTINUE;
	textout->out(Cmd[2]);
	textout->out(Cmd[3]);
	return CMD_FINISH;
}

CMD_0x1C(0x53)		//2. FS 'S'		设置汉字字间距
{
	if(CmdLen<4)return CMD_CONTINUE;
	textout->charspace_multi_left=Cmd[2];
	textout->charspace_multi_right=Cmd[3];
	return CMD_FINISH;
}

CMD_0x1C(0x57)		// FS 'W'		设置/取消汉字倍宽倍高
{
	if(CmdLen<3)return CMD_CONTINUE;
	uint8_t v = Cmd[2]&0x01;
	textout->textStyle->fsMulti->scaleX = 1+v;
	textout->textStyle->fsMulti->scaleY = 1+v;
	return CMD_FINISH;
}
CMD_0x1C(0x63)		//4. FS 'c'		设置代码页
{
	if(CmdLen<4)return CMD_CONTINUE;
	uint16_t codepage = Cmd[2]+Cmd[3]*256;
	canvas->nls->set_locale(codepage);
	return CMD_FINISH;
}

