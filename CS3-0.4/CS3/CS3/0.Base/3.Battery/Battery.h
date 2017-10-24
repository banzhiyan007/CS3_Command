#ifndef BATTERY_H
#define BATTERY_H

enum CHARGE_STATE
{
	CHARGE_STATE_IDLE=0,
	CHARGE_STATE_CHARGING=1,
	CHARGE_STATE_FINISH=2,
	CHARGE_STATE_NO_BATT=3,
};

class Battery
{
public:
	Button			*ac_ok;
	Button			*chg_ok;
	AnalogIn		*an_vbat;
	CHARGE_STATE	charge_state;
	int				battery_voltage;
	int				battery_capacity;
	int				battery_level;
	int				no_battery;
public:
	Battery();
	void update(void);
};
extern Battery *battery;

#endif

