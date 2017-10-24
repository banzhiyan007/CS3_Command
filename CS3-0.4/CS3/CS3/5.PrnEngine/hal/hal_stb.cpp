#include <mbed.h>
#include "prnEngine.h"
#include "ExtIO.h"
extern "C"
{
#include "cs_types.h"
#include "sys_ctrl.h"
#include "hal_timers.h"
};

void PrnEngine::stb_enable(int enabled)
{
	static int last_enabled=-1;
	if(last_enabled!=enabled)
	{
		last_enabled=enabled;
		stb_is_active=enabled;

		STB->write(enabled);

		if(enabled)
		{
			hal_TimWatchDogClose();
			hal_TimWatchDogOpen(16*100);
		}
		else
		{
			hal_TimWatchDogClose();
		}
	}
}

