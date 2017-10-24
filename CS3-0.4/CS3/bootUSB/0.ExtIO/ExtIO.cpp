#include "ExtIO.h"

ExtIO *extIo;

static void init_ext_io(void)
{
	extIo=new ExtIO(P1_3,P1_4,P1_0,P2_0);
}
INIT_CALL("0",init_ext_io);
//=================================================================================================================
int extIo_update_disable=0;
void extIo_update(void)
{
	if(!extIo_update_disable)extIo->update();
}

int ext_io_in_b0(void){extIo->update();return extIo->IN.bit.b0;}
int ext_io_in_b1(void){extIo->update();return extIo->IN.bit.b1;}
int ext_io_in_b2(void){extIo->update();return extIo->IN.bit.b2;}
int ext_io_in_b3(void){extIo->update();return extIo->IN.bit.b3;}
int ext_io_in_b4(void){extIo->update();return extIo->IN.bit.b4;}
int ext_io_in_b5(void){extIo->update();return extIo->IN.bit.b5;}
int ext_io_in_b6(void){extIo->update();return extIo->IN.bit.b6;}
int ext_io_in_b7(void){extIo->update();return extIo->IN.bit.b7;}

int ext_io_out_b0(int v){if(v!=-1){extIo->OUT.bit.b0=v;extIo->update();};return extIo->OUT.bit.b0;}
int ext_io_out_b1(int v){if(v!=-1){extIo->OUT.bit.b1=v;extIo->update();};return extIo->OUT.bit.b1;}
int ext_io_out_b2(int v){if(v!=-1){extIo->OUT.bit.b2=v;extIo->update();};return extIo->OUT.bit.b2;}
int ext_io_out_b3(int v){if(v!=-1){extIo->OUT.bit.b3=v;extIo->update();};return extIo->OUT.bit.b3;}
int ext_io_out_b4(int v){if(v!=-1){extIo->OUT.bit.b4=v;extIo->update();};return extIo->OUT.bit.b4;}
int ext_io_out_b5(int v){if(v!=-1){extIo->OUT.bit.b5=v;extIo->update();};return extIo->OUT.bit.b5;}
int ext_io_out_b6(int v){if(v!=-1){extIo->OUT.bit.b6=v;extIo->update();};return extIo->OUT.bit.b6;}
int ext_io_out_b7(int v){if(v!=-1){extIo->OUT.bit.b7=v;extIo->update();};return extIo->OUT.bit.b7;}

ExtIO::ExtIO(PinName dout,PinName din,PinName sclk,PinName ncs):spi(dout,din,sclk,ncs)
{
	spi.select(1);
	spi.frequency(32000000);
	OUT.data=0;
	update();
}

void ExtIO::update(void)
{
	spi.select(0);
	IN.data=spi.write(OUT.data);
	spi.select(1);
}

