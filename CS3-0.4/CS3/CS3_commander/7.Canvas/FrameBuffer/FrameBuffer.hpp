#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <block.hpp>
#include "Ram.hpp"
#include "prnCmd.h"
#include "Image.hpp"

class FrameBuffer
{
public:
	uint16_t FrameWidth;
	uint16_t FrameHeight;
	uint16_t LineByteCount;
	int      WindowLeft;
	int      WindowTop;
	int      WindowRight;
	int      WindowBottom;
	int      DataLeft;
	int      DataTop;
	int      DataRight;
	int      DataBottom;
	uint8_t *line_tmp;
	Ram		*ram;
	uint32_t pDataAddr;
	uint8_t  *pLineHaveData;
	struct 
	{
		uint8_t *pData;
		uint8_t  *invalidLineData;
		int		cacheLineCount;
		int		lineMin;
		int		lineMax;
		int		unSave;
	}cache;
	uint8_t *cache_line(int y)
	{
		if(y<0||y>=FrameHeight)return cache.invalidLineData;
		if(y<cache.lineMin||y>cache.lineMax)
		{
			cache_save();
			cache.lineMin=y;
			cache.lineMax=y+cache.cacheLineCount-1;
			if(cache.lineMax>=FrameHeight)
			{
				int offset=cache.lineMax-(FrameHeight-1);
				cache.lineMin-=offset;
				cache.lineMax-=offset;
			}
			ram->read(pDataAddr+LineByteCount*cache.lineMin,cache.pData,LineByteCount*(cache.lineMax-cache.lineMin+1));
			cache.unSave=1;
			for(int i=cache.lineMin;i<=cache.lineMax;i++)
			{
				if((pLineHaveData[i/8]&(1<<(i%8)))==0)
				{
					memset(cache.pData+LineByteCount*(i-cache.lineMin),0,LineByteCount);
				}
				pLineHaveData[i/8]|=(1<<(i%8));
			}
		}
		return cache.pData+LineByteCount*(y-cache.lineMin);
	}
	void	 cache_save(void)
	{
		if(cache.unSave)
		{
			ram->write(pDataAddr+LineByteCount*cache.lineMin,cache.pData,LineByteCount*(cache.lineMax-cache.lineMin+1));
			cache.unSave=0;
		}
	}
public:
	FrameBuffer(Ram *ram,int width,int height)
	{
		this->ram=ram;
		LineByteCount = (width==0)?0:((width-1)/8+1);
		line_tmp=(uint8_t*)malloc(LineByteCount);

		pDataAddr=(uint32_t)ram->malloc(LineByteCount*height,"FrameBuffer");
		pLineHaveData=(uint8_t *)malloc((height-1)/8+1);

		cache.cacheLineCount=48;
		cache.pData=(uint8_t*)malloc(LineByteCount*cache.cacheLineCount);
		cache.invalidLineData=(uint8_t*)malloc(LineByteCount);
		cache.lineMin=-1;
		cache.lineMax=-1;
		cache.unSave=0;

		FrameWidth = width;
		FrameHeight = height;
		WindowLeft = 0;
		WindowTop = 0;
		WindowRight = width-1;
		WindowBottom = height-1;
		clear();
	}
	int  have_data(void)
	{
		return ((DataRight-DataLeft+1)>0&&(DataBottom-DataTop+1)>0);
	}
	void clear_data_flag(void)
	{
		DataLeft = WindowRight+1;
		DataTop = WindowBottom+1;
		DataRight = WindowLeft-1;
		DataBottom = WindowTop-1;
	}
	void set_size(int width,int height)
	{
		LineByteCount = (width==0)?0:((width-1)/8+1);

		cache.lineMin=-1;
		cache.lineMax=-1;
		cache.unSave=0;

		FrameWidth = width;
		FrameHeight = height;
		WindowLeft = 0;
		WindowTop = 0;
		WindowRight = width-1;
		WindowBottom = height-1;
		clear();
	}
	void set_data_flag(void)
	{
		DataLeft = WindowLeft;
		DataTop = WindowTop;
		DataRight = WindowRight;
		DataBottom = WindowBottom;
	}
	void clear(void)
	{
		memset(pLineHaveData,0,(FrameHeight-1)/8+1);
		cache.lineMin=-1;
		cache.lineMax=-1;
		cache.unSave=0;
		clear_data_flag();
	}
	void clear_region(int x0,int y0,int x1,int y1)
	{
		if(x0<WindowLeft)x0=WindowLeft;
		if(x1>WindowRight)x1=WindowRight;
		if(y0<WindowTop)y0=WindowTop;
		if(y1>WindowBottom)y1=WindowBottom;
		if((x1-x0+1)<=0)return;
		if((y1-y0+1)<=0)return;
		if(DataLeft>x0)DataLeft=x0;
		if(DataRight<x1)DataRight=x1;
		if(DataTop>y0)DataTop=y0;
		if(DataBottom<y1)DataBottom=y1;
		uint16_t byte_x0=x0/8;
		uint16_t byte_x1=x1/8;
		uint8_t  mask_x0=~(0xFF<<(x0%8));
		uint8_t  mask_x1=(0xFF<<(x1%8+1));
		uint16_t byte_x,y;

		for(y=y0;y<=y1;y++)
		{
			uint8_t *ptr=cache_line(y);

			uint8_t save_byte_x0,save_byte_x1;
			save_byte_x0 = ptr[byte_x0]&mask_x0;
			save_byte_x1 = ptr[byte_x1]&mask_x1;
			for(byte_x=byte_x0;byte_x<=byte_x1;byte_x++)
			{
				ptr[byte_x]=0x00;
			}
			ptr[byte_x0]|=save_byte_x0;
			ptr[byte_x1]|=save_byte_x1;
		}
	}
	void fill_region(int x0,int y0,int x1,int y1)
	{
		if(x0<WindowLeft)x0=WindowLeft;
		if(x1>WindowRight)x1=WindowRight;
		if(y0<WindowTop)y0=WindowTop;
		if(y1>WindowBottom)y1=WindowBottom;
		if((x1-x0+1)<=0)return;
		if((y1-y0+1)<=0)return;
		if(DataLeft>x0)DataLeft=x0;
		if(DataRight<x1)DataRight=x1;
		if(DataTop>y0)DataTop=y0;
		if(DataBottom<y1)DataBottom=y1;
		uint16_t byte_x0=x0/8;
		uint16_t byte_x1=x1/8;
		uint8_t  mask_x0=~(0xFF<<(x0%8));
		uint8_t  mask_x1=(0xFF<<(x1%8+1));
		uint16_t byte_x,y;

		for(y=y0;y<=y1;y++)
		{
			uint8_t *ptr=cache_line(y);

			uint8_t save_byte_x0,save_byte_x1;
			save_byte_x0 = ptr[byte_x0]&mask_x0;
			save_byte_x1 = ptr[byte_x1]&mask_x1;
			for(byte_x=byte_x0;byte_x<=byte_x1;byte_x++)
			{
				ptr[byte_x]=0xFF;
			}
			ptr[byte_x0]&=~mask_x0;
			ptr[byte_x0]|=save_byte_x0;
			ptr[byte_x1]&=~mask_x1;
			ptr[byte_x1]|=save_byte_x1;
		}
	}
	void draw_image(int x,int y,Image *image,bool clear_background = true)
	{
		//if(x>WindowRight)return;
		//if((x+block->Width)<WindowLeft)return;
		//if(y>WindowBottom)return;
		//if((y+block->Height)<WindowTop)return;

		int fpl = (x >= WindowLeft)?x:WindowLeft;											//frame pos left
		int fpr = ((x+image->Width-1)<=WindowRight)?(x+image->Width-1):WindowRight;		//frame pos right
		int fpt = (y >= WindowTop)?y:WindowTop;											//frame pos top
		int fpb = ((y+image->Height-1)<=WindowBottom)?(y+image->Height-1):WindowBottom;	//frame pos bottom

		int bpl = (x >= WindowLeft)?0:(WindowLeft - x);									 //block pos left
		int bpt = (y >= WindowTop)?0:(WindowTop-y);										 //block pos top

		if(DataLeft > fpl)DataLeft = fpl;
		if(DataRight < fpr)DataRight = fpr;
		if(DataTop > fpt)DataTop = fpt;
		if(DataBottom < fpb)DataBottom = fpb;

		int BlockLineByteCount=((image->Width-1)/8+1);
		int fplb = fpl/8;
		int fprb = fpr/8;
		int bplb = bpl/8;

		int read_count=(image->bbw<LineByteCount)?image->bbw:LineByteCount;
		uint8_t *pBlockData = image->read_line_data(bpt,line_tmp,read_count);
		
		uint8_t maskl = 0xFF>>(8-(fpl%8));
		uint8_t maskr = 0xFF<<((fpr%8)+1);

		int i,j;
		for(i=fpt;i<=fpb;i++)
		{
			uint8_t *pFrameData = cache_line(i);

			unsigned char  cl,cr;
			if(clear_background)
			{
				//blank left and right
				cl = pFrameData[fplb] & maskl;
				cr = pFrameData[fprb] & maskr;
				for(j=fplb;j<=fprb;j++)pFrameData[j]=0;
				pFrameData[fplb]|=cl;
				pFrameData[fprb]|=cr;
			}
			//fill content
			cl = pFrameData[fplb];
			cr = pFrameData[fprb];
			for(j=0;j<=fprb-fplb;j++)
			{
				pFrameData[fplb+j]|=(pBlockData[bplb+j]<<(x%8))|(pBlockData[bplb+j-1]>>(8-(x%8)));
			}
			pFrameData[fplb] = (pFrameData[fplb]&(~maskl))|cl;
			pFrameData[fprb] = (pFrameData[fprb]&(~maskr))|cr;

			pBlockData=image->read_line_data(++bpt,line_tmp,read_count);
		}
	}
	void draw_image_scale(int x,int y,Image *image,int NewWidth,int NewHeight,bool clear_background = true)
	{
		if(NewWidth==0)NewWidth=image->Width;
		if(NewHeight==0)NewHeight=image->Height;

		//if(x>WindowRight)return;
		//if((x+NewWidth)<WindowLeft)return;
		//if(y>WindowBottom)return;
		//if((y+NewHeight)<WindowTop)return;

		int fpl = (x >= WindowLeft)?x:WindowLeft;													//frame pos left
		int fpr = ((x+NewWidth-1)<=WindowRight)?(x+NewWidth-1):WindowRight; 	//frame pos right
		int fpt = (y >= WindowTop)?y:WindowTop;													//frame pos top
		int fpb = ((y+NewHeight-1)<=WindowBottom)?(y+NewHeight-1):WindowBottom;//frame pos bottom

		int bpl = (x >= WindowLeft)?0:(WindowLeft - x);									 			//block pos left

		if(DataLeft > fpl)DataLeft = fpl;
		if(DataRight < fpr)DataRight = fpr;
		if(DataTop > fpt)DataTop = fpt;
		if(DataBottom < fpb)DataBottom = fpb;

		int BlockLineByteCount=((image->Width-1)/8+1);
		int fplb = fpl/8;
		int fprb = fpr/8;
		int bplb = bpl/8;
		uint8_t maskl = 0xFF>>(8-(fpl%8));
		uint8_t maskr = 0xFF<<((fpr%8)+1);

		int px,py,bx,by;
		for(py=fpt;py<=fpb;py++)
		{
			by = (py-y)*image->Height/NewHeight;

			uint8_t *pFrameData = cache_line(py);

			uint8_t *pBlockData = image->read_line_data(by,line_tmp,LineByteCount);

			if(NewWidth==image->Width)
			{
				int j;
				unsigned char  cl,cr;
				//blank left and right
				cl = pFrameData[fplb] & maskl;
				cr = pFrameData[fprb] & maskr;
				if(clear_background){for(j=fplb;j<=fprb;j++)pFrameData[j]=0;}
				pFrameData[fplb]|=cl;
				pFrameData[fprb]|=cr;
				//fill content
				cl = pFrameData[fplb];
				cr = pFrameData[fprb];
				for(j=0;j<=fprb-fplb;j++)
				{
					pFrameData[fplb+j]|=(pBlockData[bplb+j]<<(x%8))|(pBlockData[bplb+j-1]>>(8-(x%8)));
				}
				pFrameData[fplb] = (pFrameData[fplb]&(~maskl))|cl;
				pFrameData[fprb] = (pFrameData[fprb]&(~maskr))|cr;
			}
			else
			{
				for(px=fpl;px<=fpr;px++)
				{
					bx = (px-x)*image->Width/NewWidth;
					if(pBlockData[bx/8]&(1<<(bx%8)))
					{
						pFrameData[px/8]|=(1<<(px%8));
					}
					else
					{
						if(clear_background) pFrameData[px/8]&=~(1<<(px%8));
					}
				}
			}
		}
	}
	void clear_image(int x,int y,Image *image)
	{
		if(x>WindowRight)return;
		if((x+image->Width)<WindowLeft)return;
		if(y>WindowBottom)return;
		if((y+image->Height)<WindowTop)return;

		int fpl = (x >= WindowLeft)?x:WindowLeft;											//frame pos left
		int fpr = ((x+image->Width-1)<=WindowRight)?(x+image->Width-1):WindowRight;		//frame pos right
		int fpt = (y >= WindowTop)?y:WindowTop;											//frame pos top
		int fpb = ((y+image->Height-1)<=WindowBottom)?(y+image->Height-1):WindowBottom;	//frame pos bottom

		int bpt = (y >= WindowTop)?0:(WindowTop-y);										 //block pos top

		if(DataLeft > fpl)DataLeft = fpl;
		if(DataRight < fpr)DataRight = fpr;
		if(DataTop > fpt)DataTop = fpt;
		if(DataBottom < fpb)DataBottom = fpb;

		int BlockLineByteCount=((image->Width-1)/8+1);
		int fplb = fpl/8;
		int fprb = fpr/8;
		uint8_t maskl = 0xFF>>(8-(fpl%8));
		uint8_t maskr = 0xFF<<((fpr%8)+1);

		int i,j;
		for(i=fpt;i<=fpb;i++)
		{
			uint8_t *pFrameData = cache_line(i);

			unsigned char  cl,cr;
			//blank left and right
			cl = pFrameData[fplb] & maskl;
			cr = pFrameData[fprb] & maskr;
			for(j=fplb;j<=fprb;j++)pFrameData[j]=0;
			pFrameData[fplb]|=cl;
			pFrameData[fprb]|=cr;
		}
	}
	void line(int x0,int y0,int x1,int y1,int size)
	{
		if(x0==x1)
		{
			int x,y;
			if(y0>y1){y=y0;y0=y1;y1=y;}
			if(DataLeft > x0)DataLeft = x0;
			if(DataRight < x0+size-1)DataRight = x0+size-1;
			if(DataTop > y0)DataTop = y0;
			if(DataBottom < y1)DataBottom = y1;
			for(y=y0;y<=y1;y++)
			{
				uint8_t *pFrameData = cache_line(y);
				for(x=x0;x<x0+size;x++)
				{
					if(x>=WindowLeft&&x<=WindowRight&&y>=WindowTop&&y<=WindowBottom)
						pFrameData[x/8]|=1<<(x%8);
				}
			}
		}
		else if(y0==y1)
		{
			int x,y;
			if(x0>x1){x=x0;x0=x1;x1=x;}
			if(DataLeft > x0)DataLeft = x0;
			if(DataRight < x1)DataRight = x1;
			if(DataTop > y0)DataTop = y0;
			if(DataBottom < y0+size-1)DataBottom = y0+size-1;
			for(y=y0;y<y0+size;y++)
			{
				uint8_t *pFrameData = cache_line(y);
				for(x=x0;x<=x1;x++)
				{
					if(x>=WindowLeft&&x<=WindowRight&&y>=WindowTop&&y<=WindowBottom)
						pFrameData[x/8]|=1<<(x%8);
				}
			}
		}
		else
		{
			int t,x,y,dx,dy,error;
			bool flag = ((y1>=y0)?(y1-y0):(y0-y1))>((x1>=x0)?(x1-x0):(x0-x1));

			if(flag )
			{
				t=x0;x0=y0;y0=t;
				t=x1;x1=y1;y1=t;
			}

			if( x0>x1 )
			{
				t=x0;x0=x1;x1=t;
				t=y0;y0=y1;y1=t;
			}

			if(flag)
			{
				if(DataLeft > y0)DataLeft = y0;
				if(DataRight < y1+size-1)DataRight = y1+size-1;
				if(DataTop > x0)DataTop = x0;
				if(DataBottom < x1)DataBottom = x1;
			}
			else
			{
				if(DataLeft > x0)DataLeft = x0;
				if(DataRight < x1)DataRight = x1;
				if(DataTop > y0)DataTop = y0;
				if(DataBottom < y1+size-1)DataBottom = y1+size-1;
			}

			dx=x1-x0;
			dy=(y1>=y0)?(y1-y0):(y0-y1);
			error=dx/2;

			for(x=x0,y=y0;x<=x1;++x)
			{
				uint8_t *ptr;
				int rx,l;
				if(flag)
				{
					ptr = cache_line(x);
					for(l=0;l<size;l++)
					{
						rx=y+l;
						if(rx>=WindowLeft&&rx<=WindowRight&&x>=WindowTop&&x<=WindowBottom)
							ptr[rx/8]|=1<<(rx%8);
					}
				}
				else
				{
					//ptr = pData+y*LineByteCount;
					for(l=0;l<size;l++)
					{
						ptr=cache_line(y+1+l);
						//ptr+=LineByteCount;
						if(x>=WindowLeft&&x<=WindowRight&&(y+l+1)>=WindowTop&&(y+l+1)<=WindowBottom)
							ptr[x/8]|=1<<(x%8);
					}
				}
				error-=dy;
				if(error<0)
				{
					y0<y1?++y:--y;
					error+=dx;
				}
			}
		}

		if(DataLeft > DataRight)
		{
			int t;
			t=DataLeft;
			DataLeft = DataRight;
			DataRight = t;
		}

		if(DataTop > DataBottom)
		{
			int t;
			t = DataTop;
			DataTop = DataBottom;
			DataBottom = t;
		}

		if(DataLeft<WindowLeft)DataLeft=WindowLeft;
		if(DataRight>WindowRight)DataRight=WindowRight;
		if(DataTop<WindowTop)DataTop=WindowTop;
		if(DataBottom>WindowBottom)DataBottom=WindowBottom;
	}
	void line_xor(int x0,int y0,int x1,int y1,int size)
	{
		if(x0==x1)
		{
			int x,y;
			if(y0>y1){y=y0;y0=y1;y1=y;}
			if(DataLeft > x0)DataLeft = x0;
			if(DataRight < x0+size-1)DataRight = x0+size-1;
			if(DataTop > y0)DataTop = y0;
			if(DataBottom < y1)DataBottom = y1;
			for(y=y0;y<=y1;y++)
			{
				uint8_t *pFrameData = cache_line(y);
				for(x=x0;x<x0+size;x++)
				{
					if(x>=WindowLeft&&x<=WindowRight&&y>=WindowTop&&y<=WindowBottom)
						pFrameData[x/8]^=1<<(x%8);
				}
			}
		}
		else if(y0==y1)
		{
			int x,y;
			if(x0>x1){x=x0;x0=x1;x1=x;}
			if(DataLeft > x0)DataLeft = x0;
			if(DataRight < x1)DataRight = x1;
			if(DataTop > y0)DataTop = y0;
			if(DataBottom < y0+size-1)DataBottom = y0+size-1;
			for(y=y0;y<y0+size;y++)
			{
				uint8_t *pFrameData = cache_line(y);
				for(x=x0;x<=x1;x++)
				{
					if(x>=WindowLeft&&x<=WindowRight&&y>=WindowTop&&y<=WindowBottom)
						pFrameData[x/8]^=1<<(x%8);
				}
			}
		}
		else
		{
			int t,x,y,dx,dy,error;
			bool flag = ((y1>=y0)?(y1-y0):(y0-y1))>((x1>=x0)?(x1-x0):(x0-x1));

			if(flag )
			{
				t=x0;x0=y0;y0=t;
				t=x1;x1=y1;y1=t;
			}

			if( x0>x1 )
			{
				t=x0;x0=x1;x1=t;
				t=y0;y0=y1;y1=t;
			}

			if(flag)
			{
				if(DataLeft > y0)DataLeft = y0;
				if(DataRight < y1+size-1)DataRight = y1+size-1;
				if(DataTop > x0)DataTop = x0;
				if(DataBottom < x1)DataBottom = x1;
			}
			else
			{
				if(DataLeft > x0)DataLeft = x0;
				if(DataRight < x1)DataRight = x1;
				if(DataTop > y0)DataTop = y0;
				if(DataBottom < y1+size-1)DataBottom = y1+size-1;
			}

			dx=x1-x0;
			dy=(y1>=y0)?(y1-y0):(y0-y1);
			error=dx/2;

			for(x=x0,y=y0;x<=x1;++x)
			{
				uint8_t *ptr;
				int rx,l;
				if(flag)
				{
					ptr = cache_line(x);
					for(l=0;l<size;l++)
					{
						rx=y+l;
						if(rx>=WindowLeft&&rx<=WindowRight&&x>=WindowTop&&x<=WindowBottom)
							ptr[rx/8]^=1<<(rx%8);
					}
				}
				else
				{
					for(l=0;l<size;l++)
					{
						ptr = cache_line(y+1+l);
						if(x>=WindowLeft&&x<=WindowRight&&(y+l+1)>=WindowTop&&(y+l+1)<=WindowBottom)
							ptr[x/8]^=1<<(x%8);
					}
				}
				error-=dy;
				if(error<0)
				{
					y0<y1?++y:--y;
					error+=dx;
				}
			}
		}

		if(DataLeft > DataRight)
		{
			int t;
			t=DataLeft;
			DataLeft = DataRight;
			DataRight = t;
		}

		if(DataTop > DataBottom)
		{
			int t;
			t = DataTop;
			DataTop = DataBottom;
			DataBottom = t;
		}
		if(DataLeft<WindowLeft)DataLeft=WindowLeft;
		if(DataRight>WindowRight)DataRight=WindowRight;
		if(DataTop<WindowTop)DataTop=WindowTop;
		if(DataBottom>WindowBottom)DataBottom=WindowBottom;
	}
	void print(int offset=0,int height=0)
	{
		int n=0;
		for(int y=DataTop;y<=DataBottom;y++)
		{
			if(height)if(n>=height)break;
			uint8_t *ptr=cache_line(y);
			if(offset)
			{
				uint8_t *buf=(uint8_t*)malloc(LineByteCount);
				memset(buf,0,LineByteCount);
				for(int x=DataLeft;x<=DataRight;x++)
				{
					int nx=offset+x;
					if(nx>=FrameWidth)break;
					if(ptr[x/8]&(1<<(x%8)))buf[nx/8]|=(1<<(nx%8));
				}
				prnCmd->print_line(buf,LineByteCount);
				free(buf);
			}
			else
			{
				prnCmd->print_line(ptr,LineByteCount);
			}
			n++;
		}
		uint8_t *buf=(uint8_t*)malloc(LineByteCount);
		memset(buf,0,LineByteCount);
		if(height>n)prnCmd->print_line(buf,LineByteCount);
		free(buf);
	}
};

#endif

