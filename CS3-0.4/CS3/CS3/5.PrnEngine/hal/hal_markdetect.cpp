#include <mbed.h>
#include "prnEngine.h"
#include "EngineCmd.h"
#include "ExtIO.h"
extern void hal_timeout_disable(void);
extern void hal_timeout_enable(void);

#define MARK_HIS_MAX		(32*8)

#define LABEL_DETECT_GAP	(1*8)
#define LABEL_OFFSET		(19*8)

#define MARK_DETECT_GAP		(3*8)
#define MARK_OFFSET			(18*8)

#define IO_MARK_L	extIo->IN.bit.b4
#define IO_MARK_R	extIo->IN.bit.b5
#define IO_MARK_R1	extIo->IN.bit.b6
#define IO_LABEL	extIo->IN.bit.b3

static uint8_t mark_his[MARK_HIS_MAX];
static uint8_t *mark_his_end;
static int     mark_his_pos;

void PrnEngine::markdetect_reset(void)
{
	markdetect_value=0;
	memset(mark_his,0,sizeof(mark_his));
	mark_his_pos=0;
}

void PrnEngine::markdetect_value_read(void)
{
	markdetect_value=0;
	if(!IO_LABEL)markdetect_value|=MARK_LABEL;
	if(IO_MARK_L)markdetect_value|=MARK_L;
	if(IO_MARK_R)markdetect_value|=MARK_R;
	if(IO_MARK_R1)markdetect_value|=MARK_R1;
}

void PrnEngine::markdetect_scroll(void)
{
	memcpy(mark_his,mark_his+1,MARK_HIS_MAX-1);
	mark_his[MARK_HIS_MAX-1]=markdetect_value;
}

int PrnEngine::markdetected(int mark)
{
	if(mark&MARK_LABEL)
	{
		if((mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*0/2)]&
			mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*1/2)]&
			mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*2/2)]&MARK_LABEL)!=0
			&&
		   (mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*3/2)]|
		    mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*4/2)]|
			mark_his[MARK_HIS_MAX-(LABEL_OFFSET+LABEL_DETECT_GAP*5/2)]|MARK_LABEL)==0)
		{
			return 1;
		}
	}
	if(mark&MARK_L)
	{
		if((mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*0/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*1/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*2/2)]&MARK_L)!=0
			&&
		   (mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*3/2)]|
		    mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*4/2)]|
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*5/2)]|MARK_L)==0)
		{
			return 1;
		}
	}
	if(mark&MARK_R)
	{
		if((mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*0/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*1/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*2/2)]&MARK_R)!=0
			&&
			(mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*3/2)]|
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*4/2)]|
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*5/2)]|MARK_R)==0)
		{
			return 1;
		}
	}
	if(mark&MARK_R1)
	{
		if((mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*0/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*1/2)]&
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*2/2)]&MARK_R1)!=0
			&&
			(mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*3/2)]|
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*4/2)]|
			mark_his[MARK_HIS_MAX-(MARK_OFFSET+MARK_DETECT_GAP*5/2)]|MARK_R1)==0)
		{
			return 1;
		}
	}
	return 0;
}
