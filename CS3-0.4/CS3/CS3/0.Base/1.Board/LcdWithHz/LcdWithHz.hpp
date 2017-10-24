#ifndef LCD_WITH_HZ_HPP
#define LCD_WITH_HZ_HPP

class LcdWithHz:public LcdOled12832_sh1306
{
public:
	LcdWithHz(I2C *i2c):LcdOled12832_sh1306(i2c)
	{

	}
	void putc16_hz(int x,int y,uint16_t wch)
	{
		extern const char *pLcdFontHzk16;
		//uint16_t index = ((wch/256)-0xA1)*94+((wch%256)-0xA1);
		uint16_t index = ((wch/256)-0xA0)*94+((wch%256)-0xA1);
		const char *pData = pLcdFontHzk16+index*32;

		uint8_t buffer[32];
		Block block(16,16,buffer);
		memcpy(block.pData,pData,32);
		frame_buffer->draw_block(x,y,&block);
	}
};

#endif
