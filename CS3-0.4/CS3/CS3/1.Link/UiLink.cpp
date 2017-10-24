#include <mbed.h>
#include "UiLcd.h"
extern int link_fail;

static char STR_HARDWARE_ERROR[17];
static void on_language_change(int language_id)
{
	if(language_id==0)
	{
		strcpy(STR_HARDWARE_ERROR,"INTERNAL ERROR");
	}
	else if(language_id==1)
	{
		strcpy(STR_HARDWARE_ERROR,"ÄÚ²¿´íÎó");
	}
}

static void link_status_handler(int redraw)
{		
	static int last_link_fail=-1;
	if(redraw||last_link_fail!=link_fail)
	{
		uiLcd->message->show(STR_HARDWARE_ERROR,link_fail);
	}
	last_link_fail=link_fail;
}

static void init_ui_link(void)
{
	on_language_change(reg_get_u32("LANGUAGE",0));
	uiLcd->add_language_handler(new FunctionPointer1Arg(on_language_change));
	uiLcd->add_status_handler(new FunctionPointer1Arg(link_status_handler));
}

INIT_CALL("9",init_ui_link);

