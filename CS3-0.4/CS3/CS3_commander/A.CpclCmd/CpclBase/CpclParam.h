#ifndef CPCL_PARAM_H
#define CPCL_PARAM_H

struct CpclParam
{
	int started;
	int printing;
	int	offset;
	int resolutionX;
	int resolutionY;
	int height;
	int quantity;
};
extern CpclParam *cpcl_param;

#endif

