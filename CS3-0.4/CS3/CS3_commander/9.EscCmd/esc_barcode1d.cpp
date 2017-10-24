#include "CmdBase.h"
#include "Textout.h"

int Barcode1DScale;
static int Barcode1DHeight;
static int Barcode1DTextAlign;
static TextStyle *barcode1dTextStyle;

static void esc_reset(void)
{
	Barcode1DScale=3;
	Barcode1DHeight=162;
	Barcode1DTextAlign=0;
	barcode1dTextStyle->reset();
	barcode1dTextStyle->fsAsc->font=canvas->fontManager->select_font_by_id(0);
}

static void init(void)
{
	barcode1dTextStyle=new TextStyle();
	esc_reset();
	TEXTOUT_RESET_FUNC_ADD(esc_reset);
}
static INIT_CALL("9",init);

CMD_0x1D(0x68)      //GS 'h'		设置条形码高度
{
	if(CmdLen<3)return CMD_CONTINUE;
	Barcode1DHeight=Cmd[2];
	return CMD_FINISH;
}

CMD_0x1D(0x77)	    //GS 'w'		设置条形码宽度
{
	if(CmdLen<3) return CMD_CONTINUE;
	int scale=Cmd[2];
	if(scale<1)scale=1;
	if(scale>6)scale=6;
	Barcode1DScale=scale;
	return CMD_FINISH;
}

CMD_0x1D(0x48)	    //GS 'H'		选择条码可识读字符的打印位置
{
	if(CmdLen<3) return CMD_CONTINUE;
	Barcode1DTextAlign = Cmd[2]&3;
	return CMD_FINISH;
}

CMD_0x1D(0x66)	    //GS 'f'		选择条码识读字符字体
{
	if(CmdLen<3) return CMD_CONTINUE;
	barcode1dTextStyle->fsAsc->font=canvas->fontManager->select_font_by_id(Cmd[2]%2);
	return CMD_FINISH;
}

static void esc_print_barcode1d(Barcode1DType type,char *text)
{
	char *display_text=(char*)malloc(strlen(text)+32);
	Block *block_barcode=canvas->load_bardcode1d(type,text,display_text);
	int barcode_width=block_barcode->Width*Barcode1DScale;
	int barcode_height=Barcode1DHeight;
	int barcode_x_offset=0;
	int barcode_y_offset=0;
	int total_width=barcode_width;
	int total_height=barcode_height;
	int text_x_offset=0;
	int text_height=0;

	if(Barcode1DTextAlign)
	{
		int text_width=0;
		for(int i=0;i<strlen(display_text);i++)
		{
			Block *block=canvas->load_multichar(barcode1dTextStyle,display_text[i]);
			text_width+=block->Width;
			if(text_height<block->Height)text_height=block->Height;
			text_height=24;
			delete block;
		}
		if(text_width>total_width)
		{
			total_width=text_width;
			barcode_x_offset=(text_width-barcode_width)/2;
		}
		else
		{
			text_x_offset=(barcode_width-text_width)/2;
		}
		if(Barcode1DTextAlign&1)
		{
			barcode_y_offset=text_height+4;
			total_height+=text_height+4;
		}
		if(Barcode1DTextAlign&2)
		{
			total_height+=text_height+4;
		}
	}
	if(textout->cur_x+total_width>textout->margin_right+1)
	{
		textout->print_line();
	}
	canvas->draw_block_scale(textout->cur_x+barcode_x_offset,textout->cur_y+canvas->Height/2-total_height+barcode_y_offset,block_barcode,Barcode1DScale,Barcode1DHeight);
	if(Barcode1DTextAlign&1)
	{
		int x=textout->cur_x+text_x_offset;
		for(int i=0;i<strlen(display_text);i++)
		{
			Block *block=canvas->load_multichar(barcode1dTextStyle,display_text[i]);
			canvas->draw_block(x,textout->cur_y+canvas->Height/2-total_height+text_height-block->Height,block);
			x+=block->Width;
			delete block;
		}
	}
	if(Barcode1DTextAlign&2)
	{
		int x=textout->cur_x+text_x_offset;
		for(int i=0;i<strlen(display_text);i++)
		{
			Block *block=canvas->load_multichar(barcode1dTextStyle,display_text[i]);
			canvas->draw_block(x,textout->cur_y+canvas->Height/2-block->Height,block);
			x+=block->Width;
			delete block;
		}
	}
	textout->cur_x+=total_width;
	delete block_barcode;
	free(display_text);
}

CMD_0x1D(0x6B)	//GS 'k'		打印条码
{
	if(CmdLen<3) return CMD_CONTINUE;
	static uint8_t m;
	if(CmdLen == 3)
	{
		m = Cmd[2]&0x7F;
		return CMD_CONTINUE;
	}
	enum Barcode1DType type=CODE39;
	if	(m%65==0)		type=UPCA;
	else if(m%65==1)	type=UPCE;
	else if(m%65==2)	type=EAN13;
	else if(m%65==3)	type=EAN8;
	else if(m%65==4)	type=CODE39;
	else if(m%65==5)	type=ITF;
	else if(m%65==6)	type=CODABAR;
	else if(m%65==7)	type=CODE93;
	else if(m%65==8)	type=CODE128;

	if(m>=65)
	{
		static int n;
		if(CmdLen==4)
		{
			n = Cmd[3];
			return CMD_CONTINUE;
		}
		else if(CmdLen==4+n)
		{
			char *code = (char*)malloc(n+1);
			memcpy(code,&Cmd[4],n);
			code[n]=0;
			esc_print_barcode1d(type,code);
			free(code);
			return CMD_FINISH;
		}
		else return CMD_CONTINUE;
	}
	else if(m>=10&&m<=12)
	{
		extern void esc_print_qrcode(int ver,int error,char *code,int size);
		extern void esc_print_pdf417(int column,int error,int hwratio,char *code,int xsize,int ysize);
		extern void esc_print_datamatrix(char *code,int size);
		extern int QRCode_Ver;
		extern int PDF417_Column;
		extern int QRCode_Error;
		extern int PDF417_Error;
		extern int PDF417_Hwratio;

		if(Cmd[CmdLen-1]==0||Cmd[CmdLen-1]==0xFF)
		{
			int datalen = CmdLen - 4;
			char *pData = (char *)malloc(datalen+1);
			memset(pData,0,datalen+1);
			memcpy(pData,Cmd+3,datalen);
			if(m==10)esc_print_pdf417(PDF417_Column,PDF417_Error,PDF417_Hwratio,pData,Barcode1DScale,Barcode1DScale);
			if(m==11)esc_print_qrcode(QRCode_Ver,QRCode_Error,pData,Barcode1DScale);
			if(m==12)esc_print_datamatrix(pData,Barcode1DScale);
			free(pData);
			return CMD_FINISH;
		}
		else return CMD_CONTINUE;
	}
	else
	{
		if(Cmd[CmdLen-1]==0||Cmd[CmdLen-1]==0xFF)
		{
			int n=CmdLen-4;
			char *code = (char*)malloc(n+1);
			memcpy(code,&Cmd[3],n);
			code[n]=0;
			esc_print_barcode1d(type,code);
			free(code);
			return CMD_FINISH;
		}
		else return CMD_CONTINUE;
	}
	return CMD_FINISH;
}
