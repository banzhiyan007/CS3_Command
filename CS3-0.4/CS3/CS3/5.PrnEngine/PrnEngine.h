#ifndef PRN_ENGINE_H
#define PRN_ENGINE_H

#include <Button.hpp>

class PrnEngine
{
private:
	int			LineMaxBytes;
	void		cmd_put(uint8_t cmd,uint16_t param,const void *buf,int len);
	uint8_t		cmd;
	uint16_t	cmd_param;
	uint8_t		*cmd_data;
	int			cmd_data_len;
	bool		cmd_get(void);
public:
	DigitalOut  *VH_EN;
	DigitalOut  *STB;
	DigitalOut  *LATCH;
	DigitalOut  *VDETECT_EN;
	DigitalOut  *VCOVER_OPEN_DETECT;
	Button      *COVER_OPENED;
	void		stb_enable(int enabled);
	int			stb_is_active;
	void		head_tx(uint8_t *data,int len);
	void		head_latch();
	void		motor_move(void);
	void		motor_move_backward(void);
private:
	uint16_t	speed_max_interval;
	uint16_t	speed_min_interval;
	uint32_t	speed_last_interval;
	uint32_t	speed_last_move_time;
	void		speed_set(int speed);
	void		speed_move(void);
	uint32_t	speed_wait_time(void);
private:
	Timer		engine_active_timer;
	uint8_t		markdetect_value;
	void		markdetect_value_read(void);
	void		markdetect_reset();
	void		markdetect_scroll();
	int 		markdetected(int mark);
	void		status_check(void);
public:
	struct
	{
		int		HaveError;
		int		Printing;
		int		CoverOpened;
		int		NoPaper;
		int		GotoMarkFail;
		int		OverHeat;
	}status;
	Fifo		*fifo_rx;
	int  Darkness;
	int  Speed;
public:
	PrnEngine();
	int CmdRepeatCount;
	int MarkWait;
	int MotorMoved;
	int stb_heat_time;
	void engine_handler(const void *p);
};
extern PrnEngine *prnEngine;

#endif

