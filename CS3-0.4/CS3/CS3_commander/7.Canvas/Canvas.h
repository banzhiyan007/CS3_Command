#ifndef CANVAS_H
#define CANVAS_H

#include "frameBuffer.hpp"
#include "FontUserDefine.hpp"
#include "TextStyle.hpp"
#include "FontManager.hpp"
#include "NLS.hpp"
#include "barcode1d.hpp"
#include "qrcode.h"
#include "pdf417.h"
#include "datamatrix.h"

class Canvas
{
private:
	FrameBuffer *frameBuffer;
public:
	FontUserDefine *fontUserDefine;
	ImageRam *imageUser;
	Nls *nls;
	FontManager *fontManager;
	Font *font_default;
	int MaxWidth;
	int MaxHeight;
	int Width;
	int Height;
public:
	Canvas(int width,int height);
	void set_size(int width,int height);
	void clear(void);
	void print(int offset=0,int min_height=0);
	int  data_width(void);
	int  data_height(void);
	Block *load_multichar(TextStyle *style,uint16_t multi_char);
	Block *load_bardcode1d(Barcode1DType type,char *data,char *display_text);
	Block *load_qrcode(int qrcodeVer,int qrcodeRank,char *code);
	Block *load_pdf417(int colnumber,int error,int hwratio,char *code);
	Block *load_datamatrix(char *code);
	void draw_block(int x,int y,Block *block,int clear_background=0);
	void draw_block_scale(int x,int y,Block *block,int scaleX,int scaleY,int clear_background=0);
	void draw_image(int x,int y,Image *image,int clear_background=0);
	void draw_image_scale(int x,int y,Image *image,int scaleX,int scaleY,int clear_background=0);
};

extern Canvas *canvas;
extern int LINE_DOT_COUNT;

#endif

