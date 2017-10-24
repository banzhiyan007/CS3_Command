#include "CmdBase.h"
#include "Textout.h"
#include "board.h"
#include <ROMFSFileHandle.h>

static uint32_t UserImageCountMax;
static uint32_t UserImageSizeMax;
static uint32_t UserImageAddrBase;

static void init(void)
{
	ROMFSFileHandle *fh=new ROMFSFileHandle(board->romfs,"UserImages.bin");
	if(fh->FileExist)
	{
		int block_size=board->spi_flash->block_size;
		UserImageSizeMax=((4+832*832/8)+block_size-1)/block_size*block_size;
		UserImageCountMax=fh->FileSize/UserImageSizeMax;
		UserImageAddrBase=(fh->FileBaseAddr+block_size-1)/block_size*block_size;
	}
	else
	{
		UserImageCountMax=0;
	}
}
static INIT_CALL("9",init);

static void save_image(int index,ImageRam *image)
{
	if(index>=UserImageCountMax)return;
	uint32_t image_addr=UserImageAddrBase+UserImageSizeMax*index;
	int block_size=board->spi_flash->block_size;
	uint8_t *buffer=(uint8_t *)malloc(block_size);
	int pos=0;
	uint16_t width=image->Width;
	uint16_t height=image->Height;
	uint16_t width1=width^0xFFFF;
	uint16_t height1=height^0xFFFF;
	memcpy(&buffer[pos],&width,2);
	pos+=2;
	memcpy(&buffer[pos],&height,2);
	pos+=2;
	memcpy(&buffer[pos],&width1,2);
	pos+=2;
	memcpy(&buffer[pos],&height1,2);
	pos+=2;
	int total_len=image->bbw*image->Height;
	int writed=0;
	uint32_t block_index=image_addr/block_size;
	while(writed<total_len)
	{
		int len=block_size-pos;
		if(len>total_len-writed)len=total_len-writed;
		image->ram->read(image->data_addr+writed,&buffer[pos],len);
		writed+=len;
		pos=0;
		board->spi_flash->write_block(buffer,block_index);
		block_index++;
	}
	free(buffer);
}

static Image *get_image(int index)
{
	if(index>=UserImageCountMax)return NULL;
	uint32_t image_addr=UserImageAddrBase+UserImageSizeMax*index;
	uint16_t width;
	uint16_t height;
	uint16_t width1;
	uint16_t height1;
	uint8_t  buf[8];
	board->spi_flash->read_random(image_addr,buf,8);
	memcpy(&width,&buf[0],2);
	memcpy(&height,&buf[2],2);
	memcpy(&width1,&buf[4],2);
	memcpy(&height1,&buf[6],2);
	if((width1!=(width^0xFFFF))||(height1!=(height^0xFFFF)))return NULL;
	return new ImageSpiFlash(board->spi_flash,width,height,image_addr+8);
}

CMD_0x1C(0x71)      //FS 'q'      上传图片到flash
{
	if(CmdLen<3)return CMD_CONTINUE;
	static int ImageCount;
	static int ImageIndex;
	static int width;
	static int height;
	static int data_size;
	static int pos;
	static int y;
	static int cache_width;
	static int cache_pos;
	static int cache_x;
	static ImageRam *image;
	if(CmdLen==3)
	{
		ImageCount=Cmd[2];
		ImageIndex=0;
		return CMD_CONTINUE;
	}
	if(CmdLen<7)return CMD_CONTINUE;
	if(CmdLen==7)
	{
		width=(Cmd[3]+Cmd[4]*256)*8;
		height=(Cmd[5]+Cmd[6]*256)*8;
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
	uint8_t c=Cmd[7];

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
		save_image(ImageIndex,(ImageRam *)image);
		ImageIndex++;
		if(ImageIndex>=ImageCount)return CMD_FINISH;
		(*pCmdLen)=3;
		return CMD_CONTINUE;
	}
	(*pCmdLen)--;
	return CMD_CONTINUE;
}

CMD_0x1C(0x70)      //GS '/'            打印上传到内存的位图
{
	if(CmdLen<4)return CMD_CONTINUE;
	int index=Cmd[2];
	int scaleX=(Cmd[3]&1)?2:1;
	int scaleY=(Cmd[3]&2)?2:1;
	if(index>0)index=index-1;
	Image *image=get_image(index);//canvas->imageUser;
	if(image==NULL)return CMD_FINISH;

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

