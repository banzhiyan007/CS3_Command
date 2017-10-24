#include <mbed.h>
#include "Board.h"
#include "UiLcd.h"
#include "Battery.h"

Battery *battery;
static void init_battery(void){battery=new Battery;}
INIT_CALL("1",init_battery);

static void battery_status_refresh(int redraw);
static void init_battery_status(void)
{
	uiLcd->add_status_handler(new FunctionPointer1Arg(battery_status_refresh));
}
INIT_CALL("3",init_battery_status);

VAR_REG_U32(BATTERY_VOLT,BATTERY_VOLT,0,"电池电压");
VAR_REG_U32(BATTERY_CAP,BATTERY_CAP,0,"电池电量");
VAR_REG_U32(BATTERY_LEVEL,BATTERY_LEVEL,0,"电量等级");

static const Icon icon_ac_power={94,0,8,12,"\x24\x24\x24\xFF\x00\x7E\x7E\x7E\x3C\x00\x18\x30"};
static const Icon icon_battery[6]=
{
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x07\x00\x00\x01\x01\x00\x00\x01\x01\x00\x00\x01\x07\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x70\x01\x04\x00\x70\x01\x07\x00\x70\x01\x01\x00\x70\x01\x01\x00\x70\x01\x07\x00\x70\x01\x04\x00\x70\x01\x04\x00\x70\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x77\x01\x04\x00\x77\x01\x07\x00\x77\x01\x01\x00\x77\x01\x01\x00\x77\x01\x07\x00\x77\x01\x04\x00\x77\x01\x04\x00\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x70\x77\x01\x04\x70\x77\x01\x07\x70\x77\x01\x01\x70\x77\x01\x01\x70\x77\x01\x07\x70\x77\x01\x04\x70\x77\x01\x04\x70\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x77\x77\x01\x04\x77\x77\x01\x07\x77\x77\x01\x01\x77\x77\x01\x01\x77\x77\x01\x07\x77\x77\x01\x04\x77\x77\x01\x04\x77\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x74\x77\x77\x01\x74\x77\x77\x01\x77\x77\x77\x01\x71\x77\x77\x01\x71\x77\x77\x01\x77\x77\x77\x01\x74\x77\x77\x01\x74\x77\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
};
static const Icon icon_charge[6]=
{
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x07\x00\x00\x01\x01\x00\x00\x01\x01\x00\x00\x01\x07\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x70\x01\x04\x00\x70\x01\x07\x00\x70\x01\x01\x00\x70\x01\x01\x00\x70\x01\x07\x00\x70\x01\x04\x00\x70\x01\x04\x00\x70\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x77\x01\x04\x00\x77\x01\x07\x00\x77\x01\x01\x00\x77\x01\x01\x00\x77\x01\x07\x00\x77\x01\x04\x00\x77\x01\x04\x00\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x70\x77\x01\x04\x70\x77\x01\x07\x70\x77\x01\x01\x70\x77\x01\x01\x70\x77\x01\x07\x70\x77\x01\x04\x70\x77\x01\x04\x70\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x77\x77\x01\x04\x77\x77\x01\x07\x77\x77\x01\x01\x77\x77\x01\x01\x77\x77\x01\x07\x77\x77\x01\x04\x77\x77\x01\x04\x77\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
	{103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x74\x77\x77\x01\x74\x77\x77\x01\x77\x77\x77\x01\x71\x77\x77\x01\x71\x77\x77\x01\x77\x77\x77\x01\x74\x77\x77\x01\x74\x77\x77\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"},
};

static const Icon icon_chg_no_bat={103,0,25,12,"\xFC\xFF\xFF\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x07\x00\x00\x01\x01\x00\x00\x01\x01\x00\x00\x01\x07\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\x04\x00\x00\x01\xFC\xFF\xFF\x01"};

Battery::Battery()
{
	ac_ok=board->ac_ok;
	chg_ok=board->chg_ok;
	an_vbat=board->an_vbatt;
	battery_voltage=8000;
	battery_capacity=95;
	battery_level=5;
	charge_state=CHARGE_STATE_IDLE;
	no_battery=0;
}

void Battery::update(void)
{
	if(ac_ok->active())
	{
		if(battery_voltage>8600)
		{
			charge_state=CHARGE_STATE_NO_BATT;
		}
		else
		{
			charge_state=chg_ok->active()?CHARGE_STATE_FINISH:CHARGE_STATE_CHARGING;
		}
	}
	else
	{
		charge_state=CHARGE_STATE_IDLE;
	}

	uint16_t BAT_CELL_VOLT=an_vbat->read_raw();
	battery_voltage=BAT_CELL_VOLT*2;

	uint16_t volt_cap_table[11]={3449,3504,3545,3582,3620,3669,3735,3821,3916,4023,4200};
	uint16_t volt_cap_5percent=3476;

	int cap,i,j;
	uint16_t v;

	for(cap=100;cap>=10;cap-=10)
	{
		if(BAT_CELL_VOLT>=volt_cap_table[cap/10])break;
	}
	if(cap==100)
	{
	}
	else if(cap==0)
	{
		for(j=9;j>=1;j--)
		{
			if(j>=5)v=volt_cap_5percent+(volt_cap_table[1]-volt_cap_5percent)*(j-5)/5;
			else v=volt_cap_table[0]+(volt_cap_5percent-volt_cap_table[0])*j/5;
			if(BAT_CELL_VOLT>=v)break;
		}
		cap+=j;
	}
	else
	{
		i=cap/10;
		for(j=9;j>=1;j--)
		{
			v=volt_cap_table[i]+(volt_cap_table[i+1]-volt_cap_table[i])*j/10;
			if(BAT_CELL_VOLT>=v)break;
		}
		cap+=j;
	}
	battery_capacity=cap;

	uint8_t level_table[6]={0,20,40,60,80,100};
	if(battery_capacity==0)
	{
		battery_level=0;
		return;
	}
	for(i=5;i>=battery_level+1;i--)
	{
		if(battery_capacity>level_table[i-1]+5)
		{
			battery_level=i;
			return;
		}
	}
	for(i=1;i<=battery_level-1;i++)
	{
		if(battery_capacity<level_table[i]-5)
		{
			battery_level=i;
			return;
		}
	}

	BATTERY_VOLT=battery_voltage;
	BATTERY_CAP=battery_capacity;
	BATTERY_LEVEL=battery_level;
}

static void battery_status_refresh(int redraw)
{
	static bool charge_status_flag = false;
	static uint32_t tick=0;
	static Timer *timer=NULL;
	static uint32_t last_level=-1;
	if(timer==NULL){timer=new Timer;timer->start();}

	if(!redraw)if(timer->read_ms()<500)return;
	if(timer->read_ms()>=500){tick++;timer->reset();}

	if(tick%2)battery->update();
	// ----------------
	static uint8_t last_no_battery=-1;
	if(redraw||last_no_battery!=battery->no_battery)
	{
		if(battery->no_battery)
		{
			last_level=-1;
			uiLcd->lcd->display_icon(&icon_battery[0],false);
			uiLcd->lcd->display_icon(&icon_ac_power,false);
			uiLcd->lcd->display_icon(128-icon_ac_power.Width,icon_ac_power.y,&icon_ac_power);
		}
		else
		{
			uiLcd->lcd->display_icon(&icon_battery[0],false);
			uiLcd->lcd->display_icon(&icon_ac_power);
		}
	}
	last_no_battery = battery->no_battery;

	if(battery->no_battery)return;

	static uint8_t last_charge_state=0;
	if(redraw||last_charge_state!=battery->charge_state)
	{
		uiLcd->lcd->display_icon(&icon_ac_power,(battery->charge_state>0));
		last_level=-1;
		if(last_charge_state!=battery->charge_state)
		{
			uiLcd->beeper->beep(3000,100);
		}
	}
	if(battery->charge_state)sleep_clear();
	last_charge_state=battery->charge_state;

	if(battery->charge_state==CHARGE_STATE_IDLE)
	{
		if(battery->battery_level<=1)
		{
			static uint32_t last_state=-1;
			uint32_t state=1;//tick<=3||tick%2;
			if(redraw||last_level!=battery->battery_level||last_state!=state)
				uiLcd->lcd->display_icon(&icon_battery[battery->battery_level],state);
			last_state = state;
		}
		else
		{
			if(redraw||last_level!=battery->battery_level)
				uiLcd->lcd->display_icon(&icon_battery[battery->battery_level]);
		}
		last_level = battery->battery_level;
	}
	else if(battery->charge_state==CHARGE_STATE_CHARGING)
	{
		uint32_t level = battery->battery_level;
		if(level>=5)level=4;
		level = level+tick%(6-level);
		if(redraw||last_level!=level)
			uiLcd->lcd->display_icon(&icon_charge[level]);
		last_level = level;
	}
	else if(battery->charge_state==CHARGE_STATE_FINISH)
	{
		if(redraw||last_level!=battery->battery_level)uiLcd->lcd->display_icon(&icon_charge[battery->battery_level]);
		last_level = battery->battery_level;
		uiLcd->lcd->display_icon(&icon_ac_power);
	}
	else if(battery->charge_state==CHARGE_STATE_NO_BATT)
	{
		uiLcd->lcd->display_icon(&icon_chg_no_bat);
	}

	static int last_batt_low_level=-1;
	static int batt_low_message_on=0;

	if(battery->battery_level<=1&&last_batt_low_level!=battery->battery_level&&battery->charge_state==CHARGE_STATE_IDLE)
	{
		uiLcd->beeper->music("7 7 7");
		uiLcd->message->show(uiLcd->string->BATTERY_LOW,true);
		batt_low_message_on=1;
	}
	if(batt_low_message_on)
	{
		if(battery->battery_level>1||battery->charge_state!=CHARGE_STATE_IDLE)
		{
			uiLcd->message->show(uiLcd->string->BATTERY_LOW,false);
			batt_low_message_on=0;
		}
	}
	last_batt_low_level=battery->battery_level;
}

void uilcd_battery_display_poweroff(void)
{
	static uint32_t tick=0;
	static bool	redraw = true;
	battery->update();
	if(battery->charge_state==CHARGE_STATE_CHARGING)			
	{
		uint32_t level = battery->battery_level;
		if(level>=5) level=4;			
		level += tick++;
		if (level >= 5){
			tick = 0; 
			uiLcd->lcd->display_icon(54,10,&icon_charge[5]);	
		} else {
			uiLcd->lcd->display_icon(54,10,&icon_charge[level]);					
		}
		if (redraw){uiLcd->lcd->display_icon(54+26,10,&icon_ac_power); redraw = false; }                
	}else if(battery->charge_state==CHARGE_STATE_FINISH)
	{
		static uint32_t last_level=-1;

		if(last_level!=battery->battery_level)
			uiLcd->lcd->display_icon(54,10,&icon_charge[battery->battery_level]);
		if(redraw)uiLcd->lcd->display_icon(54+26,10,&icon_ac_power);
		last_level = battery->battery_level;
	}
	else if(battery->charge_state==CHARGE_STATE_IDLE)
	{
		uiLcd->btnPower->handler();
		if(!uiLcd->btnPower->active())
		{
			uiLcd->lcd->clear();
			uiLcd->lcd->update();
			extern void cpu_poweroff(void);
			cpu_poweroff();
		}
	}
	uiLcd->lcd->update();
}

