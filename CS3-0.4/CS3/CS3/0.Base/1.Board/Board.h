#ifndef BOARD_H
#define BOARD_H

#include "ExtIO.h"
#include "version.h"
#include <Beeper.hpp>
#include <Lcd.hpp>
#include <Button.hpp>
#include <Led.hpp>

class Board
{
public:
	Beeper              *beeper;
	Led					*out_lcd_reset;
	Lcd                 *lcd;
	Led					*out_ext_cpu_reset;
	Led					*out_vdetect;
	Button              *btn_power;
	Button              *btn_config;
	Button              *btn_feed;
	Button              *btn_fn;
	Button				*ac_ok;
	Button				*chg_ok;
	AnalogIn			*an_vbatt;
public:
	Board();
};
extern Board *board;

#endif

