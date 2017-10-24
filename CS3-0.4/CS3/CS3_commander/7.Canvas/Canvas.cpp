#include <mbed.h>
#include "Board.h"
#include "Canvas.h"

Canvas *canvas;
static void init_canvas(void)
{
	canvas=new Canvas(LINE_DOT_COUNT,1000*8);
}
INIT_CALL("7",init_canvas);

Canvas::Canvas(int width,int height)
{
	MaxWidth=width;
	MaxHeight=height;
	Width=width;
	Height=height;
	fontManager=new FontManager;
	font_default=fontManager->select_font_by_id(0);
	frameBuffer=new FrameBuffer(board->ram,width,height);
	fontUserDefine=new FontUserDefine(board->ram,94,48,48);
	imageUser=new ImageRam(board->ram,832,832);
	nls = new Nls("/disk_sys/unicode_113.znls");
}

void Canvas::set_size(int width,int height)
{
	if(width>MaxWidth)width=MaxWidth;
	if(height>MaxHeight)height=MaxHeight;
	this->Width=width;
	this->Height=height;
	frameBuffer->set_size(width,height);
	clear();
}

void Canvas::clear(void)
{
	frameBuffer->clear();
}

void Canvas::print(int offset,int min_height)
{
	frameBuffer->print(offset,min_height);
}

int Canvas::data_width(void)
{
	if(!frameBuffer->have_data())return 0;
	return frameBuffer->DataRight-frameBuffer->DataLeft+1;
}

int Canvas::data_height(void)
{
	if(!frameBuffer->have_data())return 0;
	return frameBuffer->DataBottom-frameBuffer->DataTop+1;
}

Block *Canvas::load_multichar(TextStyle *style,uint16_t multi_char)
{
	Block *block;
	uint16_t wch;
	FontStyle *fontStyle;
	uint8_t a=multi_char%256;
	uint8_t b=multi_char/256;
	if(b==0)
	{
		wch=nls->char_to_unicode(multi_char);
		Font *font=style->fsAsc->font;
		if(font==NULL)font=font_default;
		block = font->get_unicode(wch);
		fontStyle=style->fsAsc;
	}
	else
	{
		if((b==0xFE)&&(a>=0xA1))
		{
			block=fontUserDefine->get_char(a-0xA1);
		}
		else
		{
			wch=nls->multichar_to_unicode(multi_char);
			Font *font=style->fsMulti->font;
			if(font==NULL)font=font_default;
			block = font->get_unicode(wch);
		}
		fontStyle=style->fsMulti;
	}
	if(fontStyle->bold)block->bold();
	if(fontStyle->underline)block->underline(fontStyle->underline);
	if(fontStyle->inverse)block->inverse();
	if(fontStyle->rotate)block->rotate(fontStyle->rotate);
	if(fontStyle->scaleX!=1||fontStyle->scaleY!=1)block->scale_to(block->Width*fontStyle->scaleX,block->Height*fontStyle->scaleY);
	return block;
}

Block *Canvas::load_bardcode1d(Barcode1DType type,char *data,char *display_text)
{
	Barcode1D *bar=new Barcode1D(type,data,display_text);
	Block *block=new Block(bar->Width,bar->Height);
	memcpy(block->pData,bar->pData,((bar->Width+7)/8)*bar->Height);
	delete bar;
	return block;
}

Block *Canvas::load_qrcode(int qrcodeVer,int qrcodeRank,char *code)
{
	QRCode *bar=new QRCode(qrcodeVer,qrcodeRank,code);
	Block *block=new Block(bar->Width,bar->Height);
	memcpy(block->pData,bar->pData,((bar->Width+7)/8)*bar->Height);
	delete bar;
	return block;
}

Block *Canvas::load_pdf417(int colnumber,int error,int hwratio,char *code)
{
	PDF417 *bar=new PDF417(colnumber,error,hwratio,code);
	Block *block=new Block(bar->Width,bar->Height);
	memcpy(block->pData,bar->pData,((bar->Width+7)/8)*bar->Height);
	delete bar;
	return block;
}

Block *Canvas::load_datamatrix(char *code)
{
	Datamatrix *bar=new Datamatrix(code);
	Block *block=new Block(bar->Width,bar->Height);
	memcpy(block->pData,bar->pData,((bar->Width+7)/8)*bar->Height);
	delete bar;
	return block;
}

void Canvas::draw_block(int x,int y,Block *block,int clear_background)
{
	if(block==NULL)return;
	ImageBlock image(block);
	if(frameBuffer!=NULL)frameBuffer->draw_image(x,y,&image,clear_background);
}

void Canvas::draw_block_scale(int x,int y,Block *block,int scaleX,int scaleY,int clear_background)
{
	if(block==NULL)return;
	ImageBlock image(block);
	if(frameBuffer!=NULL)frameBuffer->draw_image_scale(x,y,&image,image.Width*scaleX,image.Height*scaleY,clear_background);
}

void Canvas::draw_image(int x,int y,Image *image,int clear_background)
{
	if(image==NULL)return;
	if(frameBuffer!=NULL)frameBuffer->draw_image(x,y,image,clear_background);
}

void Canvas::draw_image_scale(int x,int y,Image *image,int scaleX,int scaleY,int clear_background)
{
	if(image==NULL)return;
	if(frameBuffer!=NULL)frameBuffer->draw_image_scale(x,y,image,image->Width*scaleX,image->Height*scaleY,clear_background);
}

