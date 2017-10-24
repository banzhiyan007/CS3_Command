#include <mbed.h>
#include <Graphics.hpp>
#include "UiLcd.h"
#include "prnEngine.h"

class UiEngine
{
private:
	Icon icon_printing;
	Icon icon_nopaper;
	Icon icon_opencover;
private:
	char STR_DARKNESS[16+1];
	char STR_COVER_OPENED[16+1];
	char STR_NO_PAPER[16+1];
	char STR_COVER_OPENED1[16+1];
	char STR_NO_PAPER1[16+1];
	char AutoFeedArr[17];
	char STR_PRINTERSPEED[17];
	char STR_PRINT_DEBUG[17];
	void on_language_change(int language_id)
	{
		if(language_id==0)
		{
			strcpy(STR_DARKNESS,"DARKNESS");
			strcpy(STR_COVER_OPENED,"COVER OPENED");
			strcpy(STR_NO_PAPER,"NO PAPER");
			strcpy(STR_COVER_OPENED1,"COVER1 OPENED");
			strcpy(STR_NO_PAPER1,"NO PAPER1");
			strcpy(AutoFeedArr, "AutoFeed");
			strcpy(STR_PRINTERSPEED, "PRINT SPEED");
			strcpy(STR_PRINT_DEBUG,"PRINT_DEBUG");
		}
		else if(language_id==1)
		{
			strcpy(STR_DARKNESS,"打印黑度");
			strcpy(STR_COVER_OPENED,"纸仓盖未合上");
			strcpy(STR_NO_PAPER,"打印机缺纸");
			strcpy(STR_COVER_OPENED1,"纸仓盖1未合上");
			strcpy(STR_NO_PAPER1,"打印机缺纸1");
			strcpy(AutoFeedArr, "自动走纸");
			strcpy(STR_PRINTERSPEED, "打印速度");
			strcpy(STR_PRINT_DEBUG,"打印调试");
		}
	}
public:
	UiEngine()
	{
		icon_printing = (Icon){76,0,16,12,"\xF0\x0F\x10\x08\xD0\x0B\x10\x08\xD0\x0B\x10\x08\xFC\x3F\x04\x20\xFC\x3F\x08\x10\x08\x10\xF0\x0F"};
		icon_nopaper = (Icon){76,0,16,12,"\xF0\x0F\x18\x08\x1C\x08\x04\x08\x14\x0A\x24\x09\xC4\x08\xC4\x08\x24\x09\x14\x0A\x04\x08\xFC\x0F"};
		icon_opencover = (Icon){76,0,16,12,"\x80\x03\x40\x02\x20\x02\x10\x02\x08\x01\x84\x00\x42\x00\xE2\x07\x12\x04\x0A\x04\x06\x04\xFE\x07"};
		on_language_change(reg_get_u32("LANGUAGE",0));
		uiLcd->add_language_handler(new FunctionPointer1Arg(this,&UiEngine::on_language_change));
		uiLcd->add_status_handler(new FunctionPointer1Arg(this,&UiEngine::printer_status_handler));
		uiLcd->config->add_reg_u32(0,STR_PRINT_DEBUG,"PRINT_DEBUG");
		uiLcd->config->add_reg_u32(0,STR_DARKNESS,"PRINT_DARKNESS");
		uiLcd->config->add_reg_u32(0,AutoFeedArr,"AUTOFEED");
	}
	void printer_status_handler(int redraw)
	{		
		static int LastCoverOpened=false;
		static int LastNoPaper=false;
		static int LastPrinting=false;
		if(redraw)
		{
			uiLcd->lcd->display_icon(&icon_opencover,prnEngine->status.CoverOpened);				
			uiLcd->lcd->display_icon(&icon_nopaper,prnEngine->status.NoPaper);					
			uiLcd->lcd->display_icon(&icon_printing,prnEngine->status.Printing);					
		}
		if(LastCoverOpened!=prnEngine->status.CoverOpened)
		{
			uiLcd->lcd->display_icon(&icon_opencover,prnEngine->status.CoverOpened);				
			uiLcd->message->show(STR_COVER_OPENED,prnEngine->status.CoverOpened);
			if(prnEngine->status.CoverOpened)board->beeper->music("7 7 7");
		}
		if(LastNoPaper!=prnEngine->status.NoPaper){
			uiLcd->lcd->display_icon(&icon_nopaper,prnEngine->status.NoPaper);					
			uiLcd->message->show(STR_NO_PAPER,prnEngine->status.NoPaper);
			if(prnEngine->status.NoPaper)board->beeper->music("7 7 7");
		}
		if(LastPrinting!=prnEngine->status.Printing){
			uiLcd->lcd->display_icon(&icon_printing,prnEngine->status.Printing);					
		}
		LastCoverOpened=prnEngine->status.CoverOpened;
		LastNoPaper=prnEngine->status.NoPaper;
		LastPrinting=prnEngine->status.Printing;
	}
};

static void init_ui_engine(void)
{
	new UiEngine;
}

INIT_CALL("9",init_ui_engine);
