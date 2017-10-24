#ifndef BARCODE1D_H
#define BARCODE1D_H

#include <Block.hpp>

#include "barcode_39.hpp"
#include "barcode_93.hpp"
#include "barcode_codabar.hpp"
#include "barcode_code128.hpp"
#include "barcode_itf.hpp"
#include "barcode_upc.hpp"
#include "barcode_EAN.hpp"

enum Barcode1DType
{
	CODE39  = 1,
	CODE128 = 2,
	CODE93  = 3,
	CODABAR = 4,
	EAN8    = 5,
	EAN13   = 6,
	UPCA    = 7,
	UPCE    = 8,
	ITF     = 9,
};

class Barcode1D: public Block
{
public :
    Barcode1D(int type,char *pdata,char *dispStr)
    {
        need_free = false;
        Width = 0;
        Height = 0;
        pData = NULL;
		if(dispStr)dispStr[0]=0;
        switch (type)
        {
			case CODE39:
				{				
					Barcode39 *bar = new Barcode39(pdata,dispStr);
					Width = bar->Width;
					Height = bar->Height;
					pData = bar->pData;
					need_free = bar->need_free;
					bar->need_free = false;
					delete bar;
				}
				break;
			case CODE93:
				{
					Barcode93 *bar = new Barcode93(pdata,dispStr);
					Width = bar->Width;
					Height = bar->Height;
					pData = bar->pData;
					need_free = bar->need_free;
					bar->need_free = false;
					delete bar;
				}
				break;
			case CODABAR:
				{
					BarcodeCodabar *bar = new BarcodeCodabar(pdata,dispStr);
					Width = bar->Width;
					Height = bar->Height;
					pData = bar->pData;
					need_free = bar->need_free;
					bar->need_free = false;
					delete bar;
				}
				break;
			case CODE128:
				{
					BarcodeCode128 *bar = new BarcodeCode128(pdata,dispStr);
					Width = bar->Width;
					Height = bar->Height;
					pData = bar->pData;
					need_free = bar->need_free;
					bar->need_free = false;
					delete bar;
				}
				break;
			case ITF:
				{
					BarcodeITF *bar = new BarcodeITF(pdata,dispStr);
					Width = bar->Width;
					Height = bar->Height;
					pData = bar->pData;
					need_free = bar->need_free;
					bar->need_free = false;
					delete bar;
				}
				break;
			case UPCA:
				{
				BarcodeUPC *bar = new BarcodeUPC((char *)"UPCA",pdata,dispStr);
				Width = bar->Width;
				Height = bar->Height;
				pData = bar->pData;
				need_free = bar->need_free;
				bar->need_free = false;
				delete bar;
				}
				break;
			case UPCE:
				{
				char str[32];
				memset(str,0,32);
				BarcodeUPC *bar = new BarcodeUPC((char *)"UPCE",pdata,str);
				memcpy(dispStr,str+1,6);
				Width = bar->Width;
				Height = bar->Height;
				pData = bar->pData;
				need_free = bar->need_free;
				bar->need_free = false;
				delete bar;
				}
				break;
			case EAN8:
				{
				BarcodeEAN *bar = new BarcodeEAN((char *)"EAN8",pdata,dispStr);
				Width = bar->Width;
				Height = bar->Height;
				pData = bar->pData;
				need_free = bar->need_free;
				bar->need_free = false;
				delete bar;
				}
				break;
			case EAN13:
				{
				BarcodeEAN *bar = new BarcodeEAN((char *)"EAN13",pdata,dispStr);
				Width = bar->Width;
				Height = bar->Height;
				pData = bar->pData;
				need_free = bar->need_free;
				bar->need_free = false;
				delete bar;
				}
				break;
			default :
				break;
        }
    }
};


#endif
