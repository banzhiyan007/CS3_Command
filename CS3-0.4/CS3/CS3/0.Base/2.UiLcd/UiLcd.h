#ifndef UI_LCD_H
#define UI_LCD_H

#include "board.h"
#include "UiLcdString.hpp"
#include "UiLcdMessage.hpp"
#include "UiLcdConfig.hpp"

class UiLcd
{
public:
	UiLcd();
	void ThreadLoop(const void *pParam);
public:
	Beeper			*beeper;
	Lcd				*lcd;
	Button			*btnConfig;
	Button			*btnSelect;
	Button			*btnPower;
	Button			*btnFn;
	UiLcdString		*string;
	UiLcdMessage	*message;
	UiLcdConfig     *config;
private:
	list<FunctionPointer1Arg> LanguageHandlerList;
	list<FunctionPointer1Arg> HandlerList;
	list<FunctionPointer1Arg> GlobalHandlerList;
	void ui_string_language_handler(int redraw);
	void on_language_change(int language_id);
public:
	int			Redraw;
	int			IconX;
	int			IconY;
	void		icon_malloc(int w,int h){IconX+=w+1;}
public:
	int  InConfigMode(){return config->ConfigMode;}
	void add_language_handler(FunctionPointer1Arg *func){LanguageHandlerList.push_back(*func);}
	void add_global_handler(FunctionPointer1Arg *func){GlobalHandlerList.push_back(*func);}
	void add_status_handler(FunctionPointer1Arg *func){HandlerList.push_back(*func);}
	void config_add_integer(int group,const char *name,uint32_t *pValue,uint32_t min,uint32_t max,FunctionPointer *func){config->add_integer(group,name,pValue,min,max,func);}
	void config_add_enum(int group,const char *name,uint32_t *pValue,const char *pValueList,FunctionPointer *func){config->add_enum(group,name,pValue,pValueList,func);}
	void config_add_function(int group,const char *name,FunctionPointer *func,bool repeat=true){config->add_function(group,name,func,repeat?UiLcdConfig::CONFIG_FUNC_REPEAT:UiLcdConfig::CONFIG_FUNC_ONCE);}
	void config_add_display_info(int group,const char *name,FunctionPointer2Arg *func){config->add_display_info(group,name,func);}
	void config_add_quick_info(int group,const char *name,FunctionPointer2Arg *func){config->add_quick_info(group,name,func);}
	void config_add_reg_u32(int group,const char *name,const char *reg_name,FunctionPointer *func=NULL){config->add_reg_u32(group,name,reg_name,func);}
};
extern UiLcd *uiLcd;

#endif
