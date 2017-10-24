#include <mbed.h>
#include "UiLcd.h"

UiLcd *uiLcd;

static void init_ui_lcd(void){uiLcd=new UiLcd;}
INIT_CALL("2",init_ui_lcd);

__attribute__((weak)) void uilcd_power_on_check(void){}
__attribute__((weak)) bool uilcd_power_off_check(void){return false;}
__attribute__((weak)) void uilcd_power_on_display(void){}
__attribute__((weak)) void uilcd_power_off_display(void){}
__attribute__((weak)) void uilcd_power_off(void){}
__attribute__((weak)) void uilcd_sleep_handler(void){}

UiLcd::UiLcd()
{
	uiLcd=this;
	Redraw=true;
	IconX=0;
	IconY=0;
	beeper=board->beeper;
	lcd=board->lcd;
	btnConfig=board->btn_config;
	btnSelect=board->btn_feed;
	btnPower=board->btn_power;
	btnFn=board->btn_fn;

	string=new UiLcdString();
	message=new UiLcdMessage(lcd);
	config=new UiLcdConfig(lcd,btnConfig,btnSelect,btnPower,btnFn);

	config->add_reg_u32(0,"LANGUAGE","LANGUAGE");
	add_global_handler(new FunctionPointer1Arg(this,&UiLcd::ui_string_language_handler));
	add_language_handler(new FunctionPointer1Arg(this,&UiLcd::on_language_change));

	new Thread("UiLcd",this,&UiLcd::ThreadLoop,NULL,osPriorityAboveNormal,768);
	uilcd_power_on_check();
}

void UiLcd::ThreadLoop(const void *pParam)
{
	int last_config_mode=false;
	uilcd_power_on_display();
	sleep_clear();
	while(1)
	{	
		bool redraw=false;

		if(config->ConfigMode)sleep_clear();
		if(last_config_mode!=config->ConfigMode)Redraw=true;
		last_config_mode=config->ConfigMode;

		if(Redraw){redraw = true;if(redraw)lcd->clear();}

		if(uilcd_power_off_check())
		{
			if(!config->ConfigMode)
			{
				uilcd_power_off_display();
				uilcd_power_off();
			}
		}
		{
			list<FunctionPointer1Arg>::iterator handler;
			for(handler=GlobalHandlerList.begin();handler!=GlobalHandlerList.end();handler++)
			{
				(*handler).call(redraw);
			}
		}
		if(!config->ConfigMode)
		{
			list<FunctionPointer1Arg>::iterator handler;
			for(handler=HandlerList.begin();handler!=HandlerList.end();handler++)
			{
				(*handler).call(redraw);
			}
			message->status_refresh(redraw);
		}
		config->status_refresh(redraw);
		lcd->update();

		if(redraw){Redraw = false;redraw=false;}
		wait_ms(50);
		uilcd_sleep_handler();			
	}
}

void UiLcd::ui_string_language_handler(int redraw)
{
	static uint32_t language_id=-1;
	if(language_id!=reg_get_u32("LANGUAGE",0))
	{
		language_id=reg_get_u32("LANGUAGE",0);
		list<FunctionPointer1Arg>::iterator handler;
		for(handler=LanguageHandlerList.begin();handler!=LanguageHandlerList.end();handler++)
		{
			(*handler).call(language_id);
		}
	}
}

void UiLcd::on_language_change(int language_id)
{
	string->update_language();
}
