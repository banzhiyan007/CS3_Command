#include "CmdBase.h"
#include "Textout.h"

//-------------------------------------------------------------------------------------------------
CMD_0x1B(0x52)	//Select international characters
//cmd【2】多国语言，前128ASCII码的变化
{
	if(CmdLen<3)return CMD_CONTINUE;
	//if(Cmd[2]<=13)gPrintParamDefault.special_country_char=Cmd[2];
	//else gPrintParamDefault.special_country_char=0;
	if (Cmd[2] < 14){
		canvas->nls->NationalCode = Cmd[2];
		//cmd_print.NationalCode = Cmd[2];
	}else{
		canvas->nls->NationalCode = 0;
		//cmd_print.NationalCode = 0;
	}
	return CMD_FINISH;
}
//-------------------------------------------------------------------------------------------------
CMD_0x1B(0x74)	//Select character code table
{
	if(CmdLen<3)return CMD_CONTINUE;
	switch(Cmd[2])
	{
		case 0: canvas->nls->set_locale(437);break;
		case 1: canvas->nls->set_locale(437);break;
		case 2: canvas->nls->set_locale(850);break;
		case 3: canvas->nls->set_locale(860);break;
		case 4: canvas->nls->set_locale(863);break;
		case 5: canvas->nls->set_locale(865);break;
		case 6: canvas->nls->set_locale(852);break;
		case 7: canvas->nls->set_locale(857);break;
		case 8: canvas->nls->set_locale(737);break;
		case 9: canvas->nls->set_locale(866);break;
		case 10: canvas->nls->set_locale(862);break;
			
		case 11: canvas->nls->set_locale(757);break;
		case 12: canvas->nls->set_locale(437);break;
		case 13: canvas->nls->set_locale(437);break;
		case 14: canvas->nls->set_locale(1252);break;
		case 15: canvas->nls->set_locale(858);break;
		case 16: canvas->nls->set_locale(855);break;
		case 17: canvas->nls->set_locale(1251);break;
		case 18: canvas->nls->set_locale(1250);break;
		case 19: canvas->nls->set_locale(1253);break;
		case 20: canvas->nls->set_locale(1254);break;
			
		case 21: canvas->nls->set_locale(1255);break;
		case 22: canvas->nls->set_locale(1258);break;
		case 23: canvas->nls->set_locale(1257);break;
		case 24: canvas->nls->set_locale(437);break;
		case 25: canvas->nls->set_locale(437);break;
		case 26: canvas->nls->set_locale(437);break;
		case 27: canvas->nls->set_locale(437);break;
		case 28: canvas->nls->set_locale(437);break;
		case 29: canvas->nls->set_locale(437);break;
		case 30: canvas->nls->set_locale(874);break;
			
		case 31: canvas->nls->set_locale(437);break;
		case 32: canvas->nls->set_locale(437);break;
		case 33: canvas->nls->set_locale(437);break;
		case 34: canvas->nls->set_locale(437);break;
		case 35: canvas->nls->set_locale(437);break;
		case 36: canvas->nls->set_locale(437);break;
		case 37: canvas->nls->set_locale(437);break;
		case 38: canvas->nls->set_locale(437);break;
		case 39: canvas->nls->set_locale(437);break;
		case 40: canvas->nls->set_locale(720);break;
			
		case 41: canvas->nls->set_locale(1256);break;
		case 42: canvas->nls->set_locale(437);break;
		case 43: canvas->nls->set_locale(437);break;
		case 44: canvas->nls->set_locale(437);break;
		case 45: canvas->nls->set_locale(437);break;
		case 46: canvas->nls->set_locale(437);break;
		case 47: canvas->nls->set_locale(437);break;
		case 48: canvas->nls->set_locale(437);break;
		case 49: canvas->nls->set_locale(437);break;
		case 50: canvas->nls->set_locale(437);break;
			
		case 252: canvas->nls->set_locale(932);break;
		case 253: canvas->nls->set_locale(949);break;
		case 254: canvas->nls->set_locale(950);break;
		case 255: canvas->nls->set_locale(936);break;
		default:  canvas->nls->set_locale(936);		
	}
	return CMD_FINISH;
}
//-------------------------------------------------------------------------------------------------
/*
CMD_0x1B(0x1D)	//Select international characters
{
	if(gEscDataSize<3)return ESC_CONTINUE;
	if(gEscData[2]!=0x74)return ESC_FINISH;
	if(gEscDataSize<4)return ESC_CONTINUE;
	switch(gEscData[3])
	{
		case 1:set_locale(437);gPrintParam.HzDisabled=true;break;
		case 2:set_locale(936);gPrintParam.HzDisabled=false;break;
		case 3:set_locale(437);gPrintParam.HzDisabled=true;break;
		case 4:set_locale(858);gPrintParam.HzDisabled=true;break;
		case 5:set_locale(852);gPrintParam.HzDisabled=true;break;
		case 6:set_locale(860);gPrintParam.HzDisabled=true;break;
		case 7:set_locale(861);gPrintParam.HzDisabled=true;break;
		case 8:set_locale(863);gPrintParam.HzDisabled=true;break;
		case 9:set_locale(865);gPrintParam.HzDisabled=true;break;
		case 10:set_locale(866);gPrintParam.HzDisabled=true;break;
		case 11:set_locale(855);gPrintParam.HzDisabled=true;break;
		case 12:set_locale(857);gPrintParam.HzDisabled=true;break;
		case 13:set_locale(862);gPrintParam.HzDisabled=true;break;
		case 14:set_locale(864);gPrintParam.HzDisabled=true;break;
		case 15:set_locale(737);gPrintParam.HzDisabled=true;break;
		case 16:set_locale(851);gPrintParam.HzDisabled=true;break;
		case 17:set_locale(869);gPrintParam.HzDisabled=true;break;
		case 18:set_locale(928);gPrintParam.HzDisabled=true;break;
		case 19:set_locale(772);gPrintParam.HzDisabled=true;break;
		case 20:set_locale(774);gPrintParam.HzDisabled=true;break;
		case 21:set_locale(874);gPrintParam.HzDisabled=true;break;

		case 32:set_locale(1252);gPrintParam.HzDisabled=true;break;
		case 33:set_locale(1250);gPrintParam.HzDisabled=true;break;
		case 34:set_locale(1251);gPrintParam.HzDisabled=true;break;

		case 64:set_locale(3840);gPrintParam.HzDisabled=true;break;
		case 65:set_locale(3841);gPrintParam.HzDisabled=true;break;
		case 66:set_locale(3843);gPrintParam.HzDisabled=true;break;
		case 67:set_locale(3844);gPrintParam.HzDisabled=true;break;
		case 68:set_locale(3845);gPrintParam.HzDisabled=true;break;
		case 69:set_locale(3846);gPrintParam.HzDisabled=true;break;
		case 70:set_locale(3847);gPrintParam.HzDisabled=true;break;
		case 71:set_locale(3848);gPrintParam.HzDisabled=true;break;
		case 72:set_locale(1001);gPrintParam.HzDisabled=true;break;
		case 73:set_locale(2001);gPrintParam.HzDisabled=true;break;
		case 74:set_locale(3001);gPrintParam.HzDisabled=true;break;
		case 75:set_locale(3002);gPrintParam.HzDisabled=true;break;
		case 76:set_locale(3011);gPrintParam.HzDisabled=true;break;
		case 77:set_locale(3012);gPrintParam.HzDisabled=true;break;
		case 79:set_locale(3041);gPrintParam.HzDisabled=true;break;
		default:set_locale(gPrinterConfig.Locale);gPrintParam.HzDisabled=false;break;
	}
	return ESC_FINISH;
}
*/
