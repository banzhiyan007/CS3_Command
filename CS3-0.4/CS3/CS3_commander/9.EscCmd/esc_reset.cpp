#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x40)
{
    textout->reset();
    return CMD_FINISH;
}

