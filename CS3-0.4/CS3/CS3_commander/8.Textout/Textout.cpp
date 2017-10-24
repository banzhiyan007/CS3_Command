#include <mbed.h>
#include "Textout.h"

VAR_REG_U32(EPSON_MODE,EPSON_MODE,0,"EPSON_MODE");

Textout *textout;
TEXTOUT_FUNC_LIST_NODE *textout_reset_func=NULL;
static void init_textout(void)
{
	textout=new Textout;
}
INIT_CALL("8",init_textout);

void cmd_textout(char ch)
{
	textout->out(ch);
}

Textout::Textout()
{
	textStyle=new TextStyle();
	tmp=NULL;
	reset();
}

void Textout::reset_codepage(void)
{
	canvas->nls->set_locale(936);
}

void Textout::reset()
{
	if(tmp!=NULL)
	{
		free(tmp);
		tmp=NULL;
	}
	reset_codepage();
	canvas->imageUser->set_size(0,0);
	cur_x=0;
	cur_y=0;
	two_byte_char=0;
	textStyle->reset();
	margin_left=0;
	margin_right=canvas->Width;
	align=0;
	canvas->fontUserDefine->clear();
	charspace_asc=0;
	charspace_multi_left=0;
	charspace_multi_right=0;
	blank_line_height=24;
	if(EPSON_MODE)
	{
		line_height=30;
		line_space=0;
	}
	else
	{
		line_height=0;
		line_space=8;
	}

	default_char_width=12;
	
	canvas->set_size(canvas->MaxWidth,canvas->MaxHeight);

	TEXTOUT_FUNC_LIST_NODE *node=textout_reset_func;
	while(node!=NULL)
	{
		node->func();
		node=node->pNext;
	}
}

void Textout::out(uint8_t ch)
{
	uint16_t multi_char;
	if(two_byte_char)
	{
		two_byte_char+=ch;
		multi_char=two_byte_char;
		two_byte_char=0;
	}
	else
	{
		if(ch>=0x80)
		{
			two_byte_char=ch*256;
			return;
		}
		else
		{
			multi_char=ch;
		}
	}
	if(multi_char=='\r')
	{
		cur_x=0;
		return;
	}
	if(multi_char=='\n')
	{
		print_line(1);
		cur_x=0;
		return;
	}
	Block *block=canvas->load_multichar(textStyle,multi_char);
	int new_width=block->Width;
	if(multi_char<0x100)new_width+=charspace_asc;
	else new_width+=charspace_multi_left+charspace_multi_right;
	if(cur_x+new_width>(margin_right+1))
	{
		print_line();
	}
	canvas->draw_block(cur_x+charspace_multi_left,cur_y+canvas->Height/2-block->Height,block);
	cur_x+=new_width;
	delete block;
}

void Textout::print_line(bool print_blank_line)
{
	int data_height=canvas->data_height();
	int print_height=0;
	int print_offset=0;
	if(data_height==0)
	{
		if(print_blank_line)
		{
			print_height=(EPSON_MODE)?line_height:(blank_line_height+line_space);
		}
	}
	else
	{
		if(EPSON_MODE)
		{
			if(data_height<line_height)print_height=line_height;
			else print_height=0;
		}
		else
		{
			print_height=data_height+line_space;
		}

		if(align==1)
		{
			print_offset=((margin_right-margin_left+1)-canvas->data_width())/2;
		}
		else if(align==2)
		{
			print_offset=((margin_right-margin_left)-canvas->data_width());
		}

	}
	canvas->print(print_offset,print_height);
	canvas->clear();
	cur_x=margin_left;
}
