#include <mbed.h>
#include "prnEngine.h"
#include "ExtIO.h"

#define OUT1 extIo->OUT.bit.b4
#define OUT2 extIo->OUT.bit.b5
#define OUT3 extIo->OUT.bit.b6
#define OUT4 extIo->OUT.bit.b7

static const int StepPhaseTable[8][2] =
{
	{  1,  1 },
	{  0,  1 },
	{ -1,  1 },
	{ -1,  0 },
	{ -1, -1 },
	{  0, -1 },
	{  1, -1 },
	{  1,  0 },
};
static int phase=0;

void PrnEngine::motor_move(void)
{
	phase+=2;
	if(phase>=8)phase-=8;

	int a = StepPhaseTable[phase][0];
	int b = StepPhaseTable[phase][1];

	OUT1 =(a==0)?0:(a>0);
	OUT2 =(a==0)?0:(a<0);
	OUT3 =(b==0)?0:(b<0);
	OUT4 =(b==0)?0:(b>0);

	extIo->update();
}

void PrnEngine::motor_move_backward(void)
{
	phase+=6;
	if(phase>=8)phase-=8;

	int a = StepPhaseTable[phase][0];
	int b = StepPhaseTable[phase][1];

	OUT1 =(a==0)?0:(a>0);
	OUT2 =(a==0)?0:(a<0);
	OUT3 =(b==0)?0:(b<0);
	OUT4 =(b==0)?0:(b>0);

	extIo->update();
}
