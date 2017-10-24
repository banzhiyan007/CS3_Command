#include "CpclBase.h"

void cpcl_new_page(void (*tx)(char *buf,int len))
{
	canvas->set_size(576,cpcl_param->height);
}
