#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x21)		//ESC '!'		选择打印模式
{
	if(CmdLen < 3)return CMD_CONTINUE;
	uint8_t value = Cmd[2];
	// Select Font
	if(value%4 ==0 )
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0);
		textout->default_char_width=12;
	}
	else if(value%4 == 1)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x01);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0x00);
		textout->default_char_width=12;
	}
	else if(value%4 == 2)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x02);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0x02);
		textout->default_char_width=12;
	}
	else if(value%4 == 3)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x01);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0x02);
		textout->default_char_width=12;
	}
	// Select Format
	textout->textStyle->fsAsc->bold = ((value&(1<<3))!=0);
	textout->textStyle->fsAsc->scaleY = (value&(1<<4))?2:1;
	textout->textStyle->fsAsc->scaleX = (value&(1<<5))?2:1;
	textout->textStyle->fsAsc->underline = (value&(1<<7))?1:0;;
	return CMD_FINISH;
}

CMD_0x1D(0x21)		//GS '!'		放大字符
{
	if(CmdLen<3) return CMD_CONTINUE;
	uint8_t value = Cmd[2]>>4;
	textout->textStyle->fsAsc->scaleX = (value%4)+1;
	textout->textStyle->fsMulti->scaleX = (value%4)+1;
	value = Cmd[2];
	textout->textStyle->fsAsc->scaleY = (value%4)+1;
	textout->textStyle->fsMulti->scaleY = (value%4)+1;
	return CMD_FINISH;
}

CMD_0x1B(0x4D)		//ESC 'M'		选择字符字体
{
	if(CmdLen<3)return CMD_CONTINUE;
	if(Cmd[2]&(~0x33))return CMD_FINISH;
	uint8_t value = Cmd[2]%4;
	if(value ==0 )
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0);
		textout->default_char_width=12;
	}
	else if(value == 1)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x01);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0);
		textout->default_char_width=12;
	}
	else if(value == 2)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x02);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0x02);
		textout->default_char_width=12;
	}
	else if(value == 3)
	{
		textout->textStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0x01);
		textout->textStyle->fsMulti->font=canvas->fontManager->select_font_by_id(0x02);
		textout->default_char_width=12;
	}
	return CMD_FINISH;
}

CMD_0x1B(0x2D)		//ESC '-'		设置/取消下划线
{
	if(CmdLen<3) return CMD_CONTINUE;
	if((Cmd[2]&(~0x33))>0)return CMD_FINISH;
	uint8_t value = (Cmd[2]%0x30)%3;
	textout->textStyle->fsAsc->underline = value;
	return CMD_FINISH;
}

//ESC 'E'		设定/解除粗体打印
CMD_0x1B(0x45) 
{
	if(CmdLen < 3)return CMD_CONTINUE;
	textout->textStyle->fsAsc->bold = ((Cmd[2]%2)>0);
	textout->textStyle->fsMulti->bold = ((Cmd[2]%2)>0);
	return CMD_FINISH;
}

//ESC 'G'		设定/解除重叠打印
CMD_0x1B(0x47)
{
	if(CmdLen<3)return CMD_CONTINUE;
	//cmd_print.DoublePrint = ((Cmd[2]%2)>0);
	textout->textStyle->fsAsc->bold = ((Cmd[2]%2)>0);
	textout->textStyle->fsMulti->bold = ((Cmd[2]%2)>0);
	return CMD_FINISH;
}

//GS 'B'		设定/解除反白打印
CMD_0x1D(0x42)		
{
	if(CmdLen<3)return CMD_CONTINUE;
	textout->textStyle->fsAsc->inverse = ((Cmd[2]%0x30)%2)>0;
	textout->textStyle->fsMulti->inverse = ((Cmd[2]%0x30)%2)>0;
	return CMD_FINISH;
}

//ESC 'V'		设置/解除字符旋转模式
CMD_0x1B(0x56)				
{
	if(CmdLen<3) return CMD_CONTINUE;
	if(Cmd[2]&(~0x33))return CMD_FINISH;
	uint8_t value = Cmd[2]%0x30;
	textout->textStyle->fsAsc->rotate = value%4;
	textout->textStyle->fsMulti->rotate = value%4;
	return CMD_FINISH;
}

CMD_0x1B(0x20)		//ESC SP	设置字符右间距
{
	if(CmdLen<3)return CMD_CONTINUE;
	int charSpace = Cmd[2];
	textout->charspace_asc = charSpace;
	return CMD_FINISH;
}

CMD_0x1B(0x25) // 设置/取消自定义字符
{
	if(CmdLen<3)return CMD_CONTINUE;
	return CMD_FINISH;
}
CMD_0x1B(0x26) // 定义自定义字符
{
	if(CmdLen < 3)return CMD_CONTINUE;
	return CMD_FINISH;
}
CMD_0x1B(0x3F)		//ESC '?' 'n'	取消自定义字符
{
	if(CmdLen<3)return CMD_FINISH;
	return CMD_FINISH;
}
CMD_0x1B(0x7B)		//ESC '{'           // 设定/取消倒置打印
{
	if(CmdLen < 3)return CMD_CONTINUE;
	return CMD_FINISH;
}

