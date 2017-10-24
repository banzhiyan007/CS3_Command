#include "Board.h"
#include "LcdWithHz.hpp"

#define PIN_LCD_RESET		EXT_OUT(1)
#define PIN_LCD_I2C_SDA		P3_0
#define PIN_LCD_I2C_SCL		P3_1

#define PIN_BEEPER			P4_2

static int pin_btn_feed_read(void){extIo_update();return extIo->IN.bit.b2;}

#define PIN_BTN_POWER			P4_1
#define PIN_BTN_CONFIG			P3_3
#define PIN_BTN_FEED			((PinName)((uint32_t)pin_btn_feed_read))//EXT_IN(2)
#define PIN_BTN_FN				NC

#define PIN_OUT_EXT_CPU_RESET	EXT_OUT(2)
#define PIN_OUT_VDETECT			EXT_OUT(0)

#define PIN_AC_OK				EXT_IN(0)
#define PIN_CHG_OK				EXT_IN(1)
#define PIN_AN_VBAT				VBAT

Board *board;

static void init_board(void)
{
	board=new Board;
}
INIT_CALL("1",init_board);

static void init_board_after_power_on(void)
{
	board->out_ext_cpu_reset->write(1);
}
INIT_CALL("9",init_board_after_power_on);

Board::Board()
{
	board=this;

	beeper=new Beeper(PIN_BEEPER);
	btn_power=new Button(PIN_BTN_POWER,0,true);
	btn_config=new Button(PIN_BTN_CONFIG,1,false);
	btn_feed=new Button(PIN_BTN_FEED,0,false);
	btn_fn=new Button(PIN_BTN_FN,0,false);

	ac_ok=new Button(PIN_AC_OK,1,false);
	chg_ok=new Button(PIN_CHG_OK,1,false);
	an_vbatt=new AnalogIn(PIN_AN_VBAT);
	
	out_ext_cpu_reset=new Led(PIN_OUT_EXT_CPU_RESET,0);
	out_ext_cpu_reset->write(0);
	out_vdetect=new Led(PIN_OUT_VDETECT,1);
	out_vdetect->write(0);

	out_lcd_reset=new Led(PIN_LCD_RESET,0);
	out_lcd_reset->write(1);
	I2C *i2c=new I2C(PIN_LCD_I2C_SDA,PIN_LCD_I2C_SCL);
	i2c->set_speed(I2C_SPEED_3M4);
	lcd=new LcdWithHz(i2c);
}

static void beep(void){board->beeper->beep(3000,300);}
//-------------------------------------------------------------------------------------------------
REG_FUNCTION(power_reset,NULL,"重启");
REG_FUNCTION(power_off,NULL,"关机");
REG_FUNCTION(reg_value_save,reg_value_save,"保存注册表信息");
extern void reg_restore_default(void);
REG_FUNCTION(reg_restore_default,reg_restore_default,"注册表恢复默认设置");
REG_FUNCTION(beep,beep,"BEEP");
VAR_REG_U32(LANGUAGE,LANGUAGE,1,"0:ENGLISH|1:中文");
//-------------------------------------------------------------------------------------------------
VAR_REG_STRING(OEM_MANUFACTURE,16,Manufacture,MANUFACTURE,"OEM厂商名称");
VAR_REG_STRING(OEM_MODELNAME,16,ModelName,MODEL_NAME,"OEM设备名称");
VAR_REG_STRING(VERSION,16,Version,VERSION,"版本号");
//-------------------------------------------------------------------------------------------------
REG_U32(CPU_ID32,&CPU_ID32,"CPU_ID32");
