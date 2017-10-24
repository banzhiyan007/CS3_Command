#ifndef FIFO_RAM_HPP
#define FIFO_RAM_HPP

#include "Ram.hpp"

class FifoRam:public Fifo
{
private:
	Ram  *ram;
	uint8_t		*PtrRd1;
	uint8_t		*cache;
	uint32_t	cache_begin_addr;
	uint32_t	cache_end_addr;
	int readed_hold;
public:
	void readed_data_hold(int enabled)
	{
		readed_hold=enabled;
		if(!readed_hold)PtrRd1= PtrRd;
	}
	int  readed_data_len(void)
	{
		uint8_t *wr = PtrRd;
		uint8_t *rd = PtrRd1;
		uint32_t len;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		return len;
	}
	int readed_data_get(int pos,void *buf,int max_len)
	{
		uint8_t *wr = PtrRd;
		uint8_t *rd = PtrRd1+pos;
		int len;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		if(len)
		{
			if(len>max_len)len=max_len;
			uint8_t *rd1=rd+len;
			if(rd1<End)
			{
				ram->read((uint32_t)rd,(uint8_t*)buf,len);
				//memcpy(buf,rd,len);
			}
			else
			{
				rd1=rd1-(End-Begin);
				ram->read((uint32_t)rd,(uint8_t*)buf,End-rd);
				ram->read((uint32_t)Begin,(uint8_t*)buf+(End-rd),rd1-Begin);
				//memcpy(buf,rd,End-rd);
				//memcpy((uint8_t*)buf+(End-rd),Begin,rd1-Begin);
			}
		}
		return len;
	}
public:
	FifoRam(Ram *ram,int size,const char *name=""):Fifo(NULL,0)
	{
		this->ram=ram;
		Begin = (uint8_t *)ram->malloc(size,name);
		End = Begin+size;
		PtrWr = Begin;
		PtrRd = Begin;
		PtrRd1= PtrRd;
		cache=NULL;
		m_CNDTR = size;
		pCNDTR = &m_CNDTR;
		readed_hold=0;
	}
	virtual void flush(void)
	{
		uint8_t *wr = End - *(pCNDTR);
		PtrRd = wr;
		PtrRd1=PtrRd;
		readed_hold=0;
	}
	virtual int length(void)
	{
		uint8_t *wr = End - *(pCNDTR);
		uint8_t *rd = PtrRd;
		uint32_t len;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		return len;
	}
	virtual int free_length(void)
	{
		uint8_t *wr = End - *(pCNDTR);
		uint8_t *rd = PtrRd1;
		uint32_t len;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		return (End-Begin)-1-len;
	}
	virtual bool getch(uint8_t *ch)
	{
		uint8_t *wr = End - *(pCNDTR);
		uint8_t *rd = PtrRd;
		if(rd==wr)return false;
		if(cache==NULL)
		{
			cache=(uint8_t *)::malloc(256);
			cache_begin_addr=0xFFFFFFFF;
			cache_end_addr=0xFFFFFFFF;
		}
		if(((uint32_t)rd<cache_begin_addr)||((uint32_t)rd>cache_end_addr))
		{
			cache_begin_addr=(uint32_t)rd;
			cache_end_addr=cache_begin_addr+256-1;
			if(cache_end_addr>((uint32_t)wr-1))cache_end_addr=(uint32_t)wr-1;
			if(cache_end_addr>((uint32_t)End-1))cache_end_addr=(uint32_t)End-1;
			ram->read(cache_begin_addr,cache,cache_end_addr-cache_begin_addr+1);
		}
		*ch=cache[(uint32_t)rd-cache_begin_addr];
		
		rd++;
		if(rd>=End)rd=Begin;
		PtrRd = rd;
		if(!readed_hold)PtrRd1=PtrRd;
		return true;
	}
	virtual int read(void *buf,int max_len)
	{
		uint8_t *wr = End - *(pCNDTR);
		uint8_t *rd = PtrRd;
		int len;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		if(len)
		{
			if(len>max_len)len=max_len;
			uint8_t *rd1=rd+len;
			if(rd1<End)
			{
				ram->read((uint32_t)rd,(uint8_t*)buf,len);
				//memcpy(buf,rd,len);
				PtrRd = rd1;
				if(!readed_hold)PtrRd1=PtrRd;
			}
			else
			{
				rd1=rd1-(End-Begin);
				ram->read((uint32_t)rd,(uint8_t*)buf,End-rd);
				ram->read((uint32_t)Begin,(uint8_t*)buf+(End-rd),rd1-Begin);
				//memcpy(buf,rd,End-rd);
				//memcpy((uint8_t*)buf+(End-rd),Begin,rd1-Begin);
				PtrRd = rd1;
				if(!readed_hold)PtrRd1=PtrRd;
			}
		}
		return len;
	}
	virtual bool write(const void *buf,int size)
	{
		uint8_t *wr = End - *(pCNDTR);
		uint8_t *rd = PtrRd1;
		uint32_t len,freelen;
		if(rd<=wr){len = wr-rd;}else{len = (End-Begin)-(rd-wr);}
		freelen=(End-Begin)-1-len;
		if(freelen<(uint32_t)size)return false;

		uint8_t *wr1=wr+size;
		if(wr1<End)
		{
			ram->write((uint32_t)wr,(uint8_t*)buf,size);
			//memcpy(wr,buf,size);
			PtrWr = wr1;
		}
		else
		{
			wr1=wr1-(End-Begin);
			ram->write((uint32_t)wr,(uint8_t*)buf,End-wr);
			ram->write((uint32_t)Begin,(uint8_t*)buf+(End-wr),wr1-Begin);
			//memcpy(wr,buf,End-wr);
			//memcpy(Begin,(uint8_t*)buf+(End-wr),wr1-Begin);
			PtrWr = wr1;
		}
		m_CNDTR = End-PtrWr;
		return true;
	}
};


#endif

