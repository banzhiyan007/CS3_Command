#include <mbed.h>
#include "EngineCmd.h"
#include "PrnEngine.h"

void PrnEngine::status_check(void)
{
	status.CoverOpened=COVER_OPENED->active();
	if(status.CoverOpened)
	{
		status.NoPaper=0;
	}
	else
	{
		status.NoPaper=(markdetect_value==(MARK_L|MARK_R|MARK_R1));
	}
}

