#include "CmdBase.h"
#include "Textout.h"

extern uint8_t *Barcode2D_Data;
extern int Barcode2D_DataLen;
extern int Barcode2D_DataPos;

extern int QRCode_Ver;
extern int QRCode_Size;
extern int QRCode_Error;

extern int PDF417_Column;
extern int PDF417_Linecount;
extern int PDF417_Cellwidth;
extern int PDF417_Lineheight;
extern int PDF417_Error;
extern int PDF417_Mode;
extern int PDF417_Hwratio;

extern void esc_print_qrcode(int ver,int error,char *code,int size);
extern void esc_print_pdf417(int column,int error,int hwratio,char *code,int xsize,int ysize);
extern void esc_print_datamatrix(char *code,int size);

CMD_0x1D(0x28)
{
	if(CmdLen<8)return CMD_CONTINUE;
	if(Cmd[2]!=0x6B)return CMD_FINISH;
	if(CmdLen==8)
	{
		if(Cmd[5]==0x31&&Cmd[6]==0x43)
		{
			QRCode_Size=Cmd[7];
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x31&&Cmd[6]==0x45)
		{
			QRCode_Error=Cmd[7]-48;
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x31&&Cmd[6]==0x50)
		{
			if(Barcode2D_Data){free(Barcode2D_Data);Barcode2D_Data=NULL;}
			Barcode2D_DataPos=0;
			Barcode2D_DataLen=Cmd[3]+Cmd[4]*256-3;
			if(Barcode2D_DataLen==0)return CMD_FINISH;
			Barcode2D_Data=(uint8_t*)malloc(Barcode2D_DataLen+1);
			Barcode2D_Data[Barcode2D_DataLen]=0;
			return CMD_CONTINUE;
		}
		else if(Cmd[5]==0x31&&Cmd[6]==0x51)
		{
			esc_print_qrcode(0,QRCode_Error,(char*)Barcode2D_Data,QRCode_Size);
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x41)
		{
			PDF417_Column=Cmd[7];
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x42)
		{
			PDF417_Linecount=Cmd[7];
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x43)
		{
			PDF417_Cellwidth=Cmd[7];
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x44)
		{
			PDF417_Lineheight=Cmd[7];
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x45)
		{
			return CMD_CONTINUE;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x46)
		{
			return CMD_FINISH;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x50)
		{
			if(Barcode2D_Data){free(Barcode2D_Data);Barcode2D_Data=NULL;}
			Barcode2D_DataPos=0;
			Barcode2D_DataLen=Cmd[3]+Cmd[4]*256-3;
			if(Barcode2D_DataLen==0)return CMD_FINISH;
			Barcode2D_Data=(uint8_t*)malloc(Barcode2D_DataLen+1);
			Barcode2D_Data[Barcode2D_DataLen]=0;
			return CMD_CONTINUE;
		}
		else if(Cmd[5]==0x30&&Cmd[6]==0x51)
		{
			esc_print_pdf417(PDF417_Column,PDF417_Error,3,(char*)Barcode2D_Data,PDF417_Cellwidth,PDF417_Lineheight);
			return CMD_FINISH;
		}
	}
	if(CmdLen==9&&Cmd[5]==0x30&&Cmd[6]==0x45)
	{
		PDF417_Error=Cmd[8]-48;
		return CMD_FINISH;
	}
	if(Cmd[5]==0x31&&Cmd[6]==0x50)
	{
		Barcode2D_Data[Barcode2D_DataPos++]=Cmd[8];
		(*pCmdLen)--;
		if(Barcode2D_DataPos<Barcode2D_DataLen)return CMD_CONTINUE;
	}
	if(Cmd[5]==0x30&&Cmd[6]==0x50)
	{
		Barcode2D_Data[Barcode2D_DataPos++]=Cmd[8];
		(*pCmdLen)--;
		if(Barcode2D_DataPos<Barcode2D_DataLen)return CMD_CONTINUE;
	}
	return CMD_FINISH;
}

