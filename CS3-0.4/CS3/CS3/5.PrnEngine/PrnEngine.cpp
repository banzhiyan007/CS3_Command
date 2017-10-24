#include <mbed.h>
#include "board.h"
#include "ExtIO.h"
#include "comm.h"
#include "prnEngine.h"
#include "EngineCmd.h"
#include "hal_timeout.h"
extern "C" void sxr_Sleep(uint32_t Period);

#define PIN_VH_EN			P3_2
#define PIN_STB				P3_5
#define PIN_LATCH			P1_1
#define PIN_VDETECT_EN		EXT_OUT(0)
#define PIN_VOPEN_DETECT	EXT_OUT(3)
#define PIN_COVER_OPENED	P3_4

#define FIFO_RX_PRN_SIZE	(12*1024)

PrnEngine *prnEngine;
Fifo *fifo_rx_prn;
REG_BUFFER(FIFO_RX_PRN,fifo_rx_prn,"RX");

static void init_prn_engine(void)
{
	prnEngine=new PrnEngine;
}
INIT_CALL("9",init_prn_engine);

VAR_REG_U32(AUTOFEED,AUTOFEED,0,"0:0|1:1");
VAR_REG_U32(PRINT_DARKNESS,PRINT_DARKNESS,2,"1:1|2:2|3:3");
VAR_REG_U32(PRINT_SPEED,PRINT_SPEED, 0,"0:0|1:1");
VAR_REG_U32(PRINT_DEBUG,PRINT_DEBUG, 0,"0:0|1:1");

#define LINE_DOT_COUNT	832

#define MOTOR_MOVE_STEPS_PER_LINE	2
#define MOTOR_MAX_INTERVAL	2000
#define MOTOR_MIN_INTERVAL	800

static uint32_t speed_d;
static uint32_t speed_t;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EngineState
{
	STATE_WAIT_CMD,
	STATE_HANDLE_CMD,
	STATE_REPEAT_CMD,
	STATE_HEAT,
	STATE_MOTOR_MOVE,
	STATE_MOTOR_MOVE_DONE,
};

EngineState PrnState = STATE_WAIT_CMD;

#define GOTO_STATE(state)	{PrnState=(EngineState)state;continue;}
#define SET_STATE(state,us)	{PrnState=(EngineState)state;sxr_Sleep(us/64);continue;}

static void on_stb_timeout(void)
{
	prnEngine->stb_enable(false);
}

PrnEngine::PrnEngine()
{
	//PRINT_DEBUG=0;
	fifo_rx=new Fifo(FIFO_RX_PRN_SIZE);
	fifo_rx_prn=fifo_rx;
	LineMaxBytes=LINE_DOT_COUNT/8;
	cmd_data=(uint8_t*)malloc(LineMaxBytes);
	Darkness=reg_get_u32("PRINT_DARKNESS",0);
	Speed=reg_get_u32("PRINT_SPEED",0);
	speed_set(Speed);
	memset(&status,0,sizeof(status));
	PrnState=(EngineState)STATE_WAIT_CMD;

	VH_EN=new DigitalOut(PIN_VH_EN,0);
	STB=new DigitalOut(PIN_STB,0);
	LATCH=new DigitalOut(PIN_LATCH,1);

	VDETECT_EN=new DigitalOut(PIN_VDETECT_EN,1);

	VCOVER_OPEN_DETECT=new DigitalOut(PIN_VOPEN_DETECT,1);
	COVER_OPENED=new Button(PIN_COVER_OPENED,0,true);
	markdetect_reset();

	CmdRepeatCount=0;
	MarkWait=0;
	MotorMoved=0;
	stb_heat_time=0;

	extIo_update_disable=1;

	hal_timeout_init(on_stb_timeout);
	new Thread("PrnEngine",this,&PrnEngine::engine_handler,NULL,osPriorityRealtime,512);
}

void PrnEngine::speed_set(int speed)
{
	static int last_speed=-1;
	if(last_speed==speed)return;
	last_speed=speed;
	speed_max_interval = MOTOR_MAX_INTERVAL*(4-speed)/4;
	speed_min_interval = MOTOR_MIN_INTERVAL*(4-speed)/4;
	speed_last_interval = speed_max_interval;
}

void PrnEngine::speed_move(void)
{
	uint32_t now=us_ticker_read();
	speed_last_interval=now-speed_last_move_time;
	speed_last_move_time=now;
}

uint32_t PrnEngine::speed_wait_time(void)
{
	uint32_t calc_interval = speed_last_interval-128;
	if(calc_interval>speed_max_interval)calc_interval=speed_max_interval;
	if(calc_interval<speed_min_interval)calc_interval=speed_min_interval;

	uint32_t calc_wait=speed_last_move_time+calc_interval;
	uint32_t now=us_ticker_read();
	if(now>=calc_wait)return 0;
	return calc_wait-now;
}

static int sleep_us(int us)
{
	uint32_t now=us_ticker_read();
	int util=now+us;
	int tick=us/64-3;
	if(tick>0)sxr_Sleep(tick);
	while(us_ticker_read()<util);
	uint32_t d=us_ticker_read()-now;
	return d;
}

void PrnEngine::engine_handler(const void *p)
{
	while(1)
	{
	start:
		switch(PrnState)
		{
			case STATE_WAIT_CMD:
				if(status.HaveError)fifo_rx->flush();
				if(cmd_get())
				{
					if(PRINT_DEBUG)
					{
						if(cmd==LINE_CMD_PRINT)
						{
							static uint8_t *data=NULL;
							if(data==NULL)data=(uint8_t*)malloc(4+LineMaxBytes);
							memset(data,0,4+LineMaxBytes);
							data[0]=0x1F;
							data[1]=0x53;
							data[2]=cmd_data_len%256;
							data[3]=cmd_data_len/256;
							memcpy(&data[4],cmd_data,cmd_data_len);
							extern void usb_tx(uint8_t *data,int len);
							usb_tx(data,4+cmd_data_len);
							sxr_Sleep(1);
						}
						GOTO_STATE(STATE_WAIT_CMD);
					}
					GOTO_STATE(STATE_HANDLE_CMD);
				}
				else
				{
					if(engine_active_timer.read_ms()>100)
					{
						engine_active_timer.reset();
						if(status.Printing!=0)
						{
							stb_enable(false);
							VH_EN->write(0);
							VDETECT_EN->write(1);
							status.Printing=0;
						}
					}
					if(status.Printing)
					{
						SET_STATE(STATE_WAIT_CMD,128);
					}
					else
					{
						Darkness=PRINT_DARKNESS;
						Speed=PRINT_SPEED;
						VDETECT_EN->write(0);
						sxr_Sleep(1);
						extIo->update();
						markdetect_value_read();
						VDETECT_EN->write(1);
						status_check();
						SET_STATE(STATE_WAIT_CMD,100000);
					}
				}
				break;
			case STATE_HANDLE_CMD:
				if(status.HaveError){GOTO_STATE(STATE_WAIT_CMD);}
				CmdRepeatCount = 1;
				switch(cmd)
				{
				case LINE_CMD_FEED:
					MarkWait = cmd_data[0];
					if(MarkWait)status.GotoMarkFail=0;
					CmdRepeatCount = cmd_param*MOTOR_MOVE_STEPS_PER_LINE;
					break;
				case LINE_CMD_FEED_BACKWARD:
					MarkWait = cmd_data[0];
					CmdRepeatCount = cmd_param*MOTOR_MOVE_STEPS_PER_LINE;
					break;
				case LINE_CMD_PRINT:
					CmdRepeatCount = MOTOR_MOVE_STEPS_PER_LINE;
					stb_heat_time=cmd_param;
					break;
				case LINE_CMD_DELAY:
					CmdRepeatCount = cmd_param;
					break;
				case LINE_CMD_DARKNESS:
					Darkness=cmd_param;
					break;
				case LINE_CMD_SPEED:
					Speed=cmd_param;
					speed_set(Speed);
					break;
				case LINE_CMD_DEFAULT_DARKNESS:
					Darkness=reg_get_u32("PRINT_DARKNESS",0);
					Speed=reg_get_u32("PRINT_SPEED",0);
					break;
				default:
					break;
				}
				GOTO_STATE(STATE_REPEAT_CMD);
			case STATE_REPEAT_CMD:
				sleep_clear();
				engine_active_timer.reset();
				if(status.Printing!=1)
				{
					VH_EN->write(1);
					VDETECT_EN->write(0);
					status.Printing=1;
					SET_STATE(STATE_REPEAT_CMD,100000);		//等待EnginePower稳定后再处理
				}
				if(CmdRepeatCount--<=0)
				{
					GOTO_STATE(STATE_WAIT_CMD);
				}
				if(status.HaveError)
				{
					stb_enable(false);
					GOTO_STATE(STATE_WAIT_CMD);
				}
				switch(cmd)
				{
				case LINE_CMD_FEED:
					if(MotorMoved%MOTOR_MOVE_STEPS_PER_LINE==0)
					{							
						if(markdetected(MarkWait)){GOTO_STATE(STATE_WAIT_CMD);}
						if(MarkWait&&CmdRepeatCount<=2)status.GotoMarkFail=1;
					}
					GOTO_STATE(STATE_MOTOR_MOVE);
				case LINE_CMD_FEED_BACKWARD:
					GOTO_STATE(STATE_REPEAT_CMD);
				case LINE_CMD_PRINT:
					while(stb_is_active)sxr_Sleep(1);
					if(CmdRepeatCount%MOTOR_MOVE_STEPS_PER_LINE==0)
					{
						head_tx(cmd_data,cmd_data_len);
						head_latch();
						GOTO_STATE(STATE_HEAT);
					}
					else
					{
						GOTO_STATE(STATE_HEAT);
					}
				case LINE_CMD_DELAY:
					SET_STATE(STATE_REPEAT_CMD,1000);
					return;
				default:
					break;
				}
				break;
			case STATE_HEAT:
				stb_enable(true);
				hal_timeout_start(stb_heat_time);
				GOTO_STATE(STATE_MOTOR_MOVE);
				return;
			case STATE_MOTOR_MOVE:
				{
					int wait_time=speed_wait_time();
					if(wait_time==0){GOTO_STATE(STATE_MOTOR_MOVE_DONE);}
					else {sleep_us(wait_time);GOTO_STATE(STATE_MOTOR_MOVE_DONE);}//{SET_STATE(STATE_MOTOR_MOVE_DONE,wait_time);}
				}
				return;
			case STATE_MOTOR_MOVE_DONE:
				motor_move();
				speed_move();
				MotorMoved++;
				if(MotorMoved%MOTOR_MOVE_STEPS_PER_LINE==0)
				{
					markdetect_value_read();
					status_check();
					markdetect_scroll();
				}
				GOTO_STATE(STATE_REPEAT_CMD);
			default:
				break;
		}
	}
}


bool PrnEngine::cmd_get(void)
{
	static enum
	{
		WAIT_FLAG,
		WAIT_CMD,
		WAIT_PARAM_L,
		WAIT_PARAM_H,
		WAIT_LEN_L,
		WAIT_LEN_H,
		WAIT_DATA,
	}state=WAIT_FLAG;
	static Timer timer;
	static int data_active;
	static int readed;

	if(timer.read_ms()>500){timer.reset();state=WAIT_FLAG;}
	uint8_t ch;
	while(fifo_rx->getch(&ch))
	{
		data_active=true;

		sleep_clear();
		timer.reset();
		switch(state)
		{
		case WAIT_FLAG:
			if(ch==0x1F)state=WAIT_CMD;
			break;
		case WAIT_CMD:
			cmd = ch;
			state = WAIT_PARAM_L;
			break;
		case WAIT_PARAM_L:
			cmd_param = ch;
			state = WAIT_PARAM_H;
			break;
		case WAIT_PARAM_H:
			cmd_param += ch*256;
			state = WAIT_LEN_L;
			break;
		case WAIT_LEN_L:
			cmd_data_len = ch;
			state = WAIT_LEN_H;
			break;
		case WAIT_LEN_H:
			cmd_data_len = cmd_data_len + ch*256;
			readed = 0;
			if(cmd_data_len==0)
			{
				state=WAIT_FLAG;
				return true;
			}
			if(cmd_data_len>LineMaxBytes)state=WAIT_FLAG;
			else state=WAIT_DATA;
			break;
		case WAIT_DATA:
			cmd_data[readed++]=ch;
			if(readed>=cmd_data_len)
			{
				state=WAIT_FLAG;
				return true;
			}
			break;
		default:
			break;
		}
	}
	return false;
}
