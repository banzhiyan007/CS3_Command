#include "CmdBase.h"
#include "Textout.h"

CMD_0x1B(0x2A)        //ESC '*'          选择位图模式
{
	if(CmdLen<5)return CMD_CONTINUE;
	static int scaleX;
	static int width;
	static int height;
	static int data_size;
	static int pos;
	static int x;
	static int y;
	static int block_bbw;
	if(CmdLen==5)
	{
		scaleX=(Cmd[2]&0x01)?1:2;
		int data_width=Cmd[3]+Cmd[4]*256;
		width=data_width;
		height=(Cmd[2]&0x20)?24:8;
		if(textout->cur_x+width*scaleX>textout->margin_right)
		{
			textout->print_line();
		}
		if(width*scaleX>(textout->margin_right+1)-textout->cur_x)width=((textout->margin_right+1)-textout->cur_x)/scaleX;
		data_size=data_width*height/8;
		pos=0;
		x=0;
		y=0;
		block_bbw=(width+7)/8;
		if(textout->tmp!=NULL){free(textout->tmp);textout->tmp=NULL;}
		textout->tmp=(uint8_t*)malloc(block_bbw*height);
		memset(textout->tmp,0,block_bbw*height);
		return CMD_CONTINUE;
	}
	if(x<width)
	{
		uint8_t c=Cmd[5];
		uint8_t *ptr=textout->tmp+block_bbw*y;
		for(int i=0;i<8;i++)
		{
			if(c&(0x80>>i))ptr[x/8]|=(1<<(x%8));
			ptr+=block_bbw;
		}
	}
	y+=8;
	if(y>=height)
	{
		y=0;
		x++;
	}
	pos++;
	if(pos>=data_size)
	{
		Block *block=new Block(width,height,textout->tmp);
		if(scaleX==1)
		{
			canvas->draw_block(textout->cur_x,textout->cur_y+canvas->Height/2-block->Height,block);
			textout->cur_x+=width;
		}
		else 
		{
			canvas->draw_block_scale(textout->cur_x,textout->cur_y+canvas->Height/2-block->Height,block,scaleX,1);
			textout->cur_x+=width*scaleX;
		}
		delete block;
		if(textout->tmp!=NULL){free(textout->tmp);textout->tmp=NULL;}
		return CMD_FINISH;
	}
	(*pCmdLen)--;
	return CMD_CONTINUE;
}

CMD_0x1D(0x2A)      //GS '*'            上传图片到内存
{
	if(CmdLen<4)return CMD_CONTINUE;
	static int width;
	static int height;
	static int data_size;
	static int pos;
	static int y;
	static int cache_width;
	static int cache_pos;
	static int cache_x;
	static ImageRam *image;
	if(CmdLen==4)
	{
		width=Cmd[2]*8;
		height=Cmd[3]*8;
		data_size=width*height/8;
		pos=0;
		y=0;
		if(textout->tmp!=NULL){free(textout->tmp);textout->tmp=NULL;}
		cache_width=(4096*8/height)/8*8;
		if(cache_width>width)cache_width=width;
		textout->tmp=(uint8_t*)malloc(cache_width*height/8);
		cache_pos=0;
		cache_x=0;
		memset(textout->tmp,0,cache_width*height/8);
		image=canvas->imageUser;
		image->set_size(width,height);
		return CMD_CONTINUE;
	}

	uint8_t c=Cmd[4];
	uint8_t *ptr=textout->tmp+cache_width/8*y;
	for(int i=0;i<8;i++)
	{
		if(c&(0x80>>i))ptr[cache_x/8]|=(1<<(cache_x%8));
		ptr+=cache_width/8;
	}
	y+=8;
	if(y>=height)
	{
		y=0;
		cache_x++;
		if(cache_x>=cache_width)
		{
			uint8_t  *ptr=textout->tmp;
			for(int i=0;i<height;i++)
			{
				image->write_line_data(i,ptr,cache_width/8,cache_pos/8);
				ptr+=cache_width/8;
			}
			memset(textout->tmp,0,cache_width*height/8);
			cache_x=0;
			cache_pos+=cache_width;
		}
	}

	pos++;
	if(pos>=data_size)
	{
		if(cache_x!=0)
		{
			uint8_t  *ptr=textout->tmp;
			int data_len=(cache_x+7)/8;
			for(int i=0;i<height;i++)
			{
				image->write_line_data(i,ptr,data_len,cache_pos/8);
				ptr+=cache_width/8;
			}
		}
		if(textout->tmp!=NULL){free(textout->tmp);textout->tmp=NULL;}
		return CMD_FINISH;
	}
	(*pCmdLen)--;
	return CMD_CONTINUE;
}

CMD_0x1D(0x2F)      //GS '/'            打印上传到内存的位图
{
	if(CmdLen<3)return CMD_CONTINUE;
	int scaleX=(Cmd[2]&1)?2:1;
	int scaleY=(Cmd[2]&2)?2:1;
	ImageRam *image=canvas->imageUser;
	if(image->Width==0||image->Height==0)return CMD_FINISH;

	if(textout->cur_x+image->Width*scaleX>textout->margin_right)
	{
		textout->print_line();
	}
	if(scaleX==1&&scaleY==1)
	{
		canvas->draw_image(textout->cur_x,textout->cur_y+canvas->Height/2-image->Height,image);
		textout->cur_x+=image->Width;
	}
	else 
	{
		canvas->draw_image_scale(textout->cur_x,textout->cur_y+canvas->Height/2-image->Height,image,scaleX,scaleY);
		textout->cur_x+=image->Width*scaleX;
	}
	return CMD_FINISH;
}

