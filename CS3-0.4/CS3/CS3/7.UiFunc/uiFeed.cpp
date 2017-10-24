#include <mbed.h>
#include "board.h"
#include "UiLcd.h"
#include "prnEngine.h"
#include "prnCmd.h"

VAR_REG_U32(LABEL_FEED_LEN,LABEL_FEED_LEN, 300*8,"");

static void feed_handler(int redraw)
{
	if(uiLcd->InConfigMode())return;
	if(prnEngine->status.Printing)return;

	static int allow_long_feed=0;

	if(!board->btn_feed->active())allow_long_feed=0;
	if(board->btn_feed->pressed())
	{
		prnCmd->feed(5*8);
		allow_long_feed=1;
	}
	if(board->btn_feed->down_time()>1000&&allow_long_feed)
	{
		board->beeper->beep(2500,80);
		prnCmd->feed(5*8);
		prnCmd->goto_mark_all(reg_get_u32("LABEL_FEED_LEN",0));
		allow_long_feed=0;
	}
}

static void init_ui_feed(void)
{
	uiLcd->add_status_handler(new FunctionPointer1Arg(feed_handler));
}

INIT_CALL("9",init_ui_feed);
