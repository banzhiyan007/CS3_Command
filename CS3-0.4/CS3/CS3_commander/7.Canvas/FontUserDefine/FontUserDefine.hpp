#ifndef FONT_USER_DEFINE_HPP
#define FONT_USER_DEFINE_HPP

#include <block.hpp>
#include "Ram.hpp"

class FontUserDefine
{
private:
	int char_count;
	int max_width;
	int max_height;
	int max_size;
	Ram *ram;
	uint8_t *pInfo;
	uint8_t *pData;
public:
	FontUserDefine(Ram *ram,int char_count,int max_width,int max_height)
	{
		this->char_count=char_count;
		this->max_width=max_width;
		this->max_height=max_height;
		this->ram=ram;
		pInfo=(uint8_t*)malloc(char_count*2);
		max_size=((max_width+7)/8)*max_height;
		pData=(uint8_t*)ram->malloc(char_count*max_size,"FontUserDefine");
		clear();
	}
	void clear(void)
	{
		memset(pInfo,0,char_count*2);
	}
	void clear_char(int index)
	{
		if(index>=char_count)return;
		pInfo[index*2+0]=0;
		pInfo[index*2+1]=0;
	}
	Block *get_char(int index)
	{
		if(index>=char_count)return NULL;
		int width=pInfo[index*2+0];
		int height=pInfo[index*2+1];
		int data_size=((width+7)/8)*height;
		Block *pBlock = new Block(width,height);
		ram->read((uint32_t)(pData+index*max_size),pBlock->pData,data_size);
		return pBlock;
	}
	void set_char(int index,int width,int height,uint8_t *data)
	{
		if(index>=char_count)return;
		pInfo[index*2+0]=width;
		pInfo[index*2+1]=height;
		int data_size=((width+7)/8)*height;
		ram->write((uint32_t)(pData+index*max_size),data,data_size);
	}
};
extern FontUserDefine *fontUserDefine;

#endif

