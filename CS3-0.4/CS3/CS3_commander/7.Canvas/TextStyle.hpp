#ifndef TEXT_STYLE_H
#define TEXT_STYLE_H

#include "Canvas.h"
#include "FontManager.hpp"

class FontStyle
{
public:
	Font *font;
	int scaleX;
	int scaleY;
	uint8_t bold;
	uint8_t underline;
	uint8_t inverse;
	uint8_t rotate;
	FontStyle(){reset();}
	void reset(void)
	{
		font=NULL;
		scaleX=1;
		scaleY=1;
		bold=0;
		underline=0;
		inverse=0;
		rotate=0;
	}
};

class TextStyle
{
public:
	FontStyle *fsMulti;
	FontStyle *fsAsc;
	TextStyle()
	{
		fsMulti=new FontStyle();
		fsAsc=new FontStyle();
		reset();
	}
	void reset(void)
	{
		fsMulti->reset();
		fsAsc->reset();
	}
};

#endif

