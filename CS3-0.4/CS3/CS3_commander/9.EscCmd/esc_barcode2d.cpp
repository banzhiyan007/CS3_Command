#include "CmdBase.h"
#include "Textout.h"

extern int Barcode1DScale;

uint8_t *Barcode2D_Data;
int Barcode2D_DataLen;
int Barcode2D_DataPos;

int QRCode_Ver;
int QRCode_Size;
int QRCode_Error;

int PDF417_Column;
int PDF417_Linecount;
int PDF417_Cellwidth;
int PDF417_Lineheight;
int PDF417_Error;
int PDF417_Mode;
int PDF417_Hwratio;

static int barcodetype;
static void esc_reset(void)
{
	if(Barcode2D_Data){free(Barcode2D_Data);Barcode2D_Data=NULL;}
	Barcode2D_DataLen=0;
	Barcode2D_DataPos=0;

	barcodetype=10;

	QRCode_Ver=0;
	QRCode_Size=3;
	QRCode_Error=0;

	PDF417_Column=0;
	PDF417_Linecount=0;
	PDF417_Cellwidth=3;
	PDF417_Lineheight=3;
	PDF417_Error=0;
	PDF417_Hwratio=3;
}

static void init(void)
{
	Barcode2D_Data=NULL;
	esc_reset();
	TEXTOUT_RESET_FUNC_ADD(esc_reset);
}
static INIT_CALL("9",init);

void esc_print_qrcode(int ver,int error,char *code,int size)
{
	Block *bar=canvas->load_qrcode(ver,error,code);
	if(textout->cur_x+bar->Width*size>textout->margin_right+1)
	{
		textout->print_line();
	}
	canvas->draw_block_scale(textout->cur_x,textout->cur_y+canvas->Height/2-bar->Height*size,bar,size,size);
	textout->cur_x+=bar->Width*size;
	delete bar;
}

void esc_print_pdf417(int column,int error,int hwratio,char *code,int xsize,int ysize)
{
	Block *bar=canvas->load_pdf417(column,error,hwratio,code);
	if(textout->cur_x+bar->Width*xsize>textout->margin_right+1)
	{
		textout->print_line();
	}
	canvas->draw_block_scale(textout->cur_x,textout->cur_y+canvas->Height/2-bar->Height*ysize,bar,xsize,ysize);
	textout->cur_x+=bar->Width*xsize;
	delete bar;
}

void esc_print_datamatrix(char *code,int size)
{
	Block *bar=canvas->load_datamatrix(code);
	if(textout->cur_x+bar->Width*size>textout->margin_right+1)
	{
		textout->print_line();
	}
	canvas->draw_block_scale(textout->cur_x,textout->cur_y+canvas->Height/2-bar->Height*size,bar,size,size);
	textout->cur_x+=bar->Width*size;
	delete bar;
}

CMD_0x1D(0x6F)	//GS 'o'		设置QRcode 字符版本，或者PDF417 列数
{
	if(CmdLen<3)return CMD_CONTINUE;
	QRCode_Ver = Cmd[2];
	PDF417_Column = Cmd[2];
	return CMD_FINISH;
}
CMD_0x1D(0x70)	//GS 'p'		设置pdf417 长宽比
{
	if(CmdLen<3)return CMD_CONTINUE;
	PDF417_Hwratio=(int)Cmd[2];
	return CMD_FINISH;
}
CMD_0x1D(0x71)	//GS 'q'		设置pdf417,QRcode 纠错级别
{
	if(CmdLen<3)return CMD_CONTINUE;
	QRCode_Error=Cmd[2];
	PDF417_Error=Cmd[2];
	return CMD_FINISH;
}

CMD_0x1D(0x5A)
{
	if (CmdLen < 3) return CMD_CONTINUE;
	if (Cmd[2] == 0){
		barcodetype = 11;	//pdf417
	}else if (Cmd[2] == 1){
		barcodetype = 12;	//DataMatrix
	}else if (Cmd[2] == 2){
		barcodetype = 10;  // QR
	}else
		barcodetype = 10;
	return CMD_FINISH;
}

CMD_0x1B(0x5A)
{
	int size;
	if(CmdLen < 7)return CMD_CONTINUE;
	int datalen = Cmd[5] + Cmd[6]*256;
	if (CmdLen < 7 + datalen) return CMD_CONTINUE;
	char *code=(char *)&Cmd[7];

	if(barcodetype==10)
	{
		int ver=Cmd[2];
		int error=Cmd[3];
		esc_print_qrcode(ver,error,code,Barcode1DScale);
	}
	else if(barcodetype==11)
	{
		int column=Cmd[2];
		int error=Cmd[3];
		int hwratio=Cmd[4];
		esc_print_pdf417(column,error,hwratio,code,Barcode1DScale,Barcode1DScale);
	}
	else if(barcodetype==12)
	{
		esc_print_datamatrix(code,Barcode1DScale);
	}
	return CMD_FINISH;
}