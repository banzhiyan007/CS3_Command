#ifndef RAM_HPP
#define RAM_HPP

struct RAM_MALLOC_INFO 
{
	const char *name;
	uint32_t addr;
	uint32_t size;
	RAM_MALLOC_INFO *pNext;
};

class Ram
{
public:
	RAM_MALLOC_INFO *malloc_info;
	uint32_t size;
	uint32_t free_size;
public:
	virtual void write(uint32_t addr,uint8_t *data,int len)
	{
		memcpy((void*)addr,data,len);
	}
	virtual void read(uint32_t addr,uint8_t *data,int len)
	{
		memcpy(data,(void*)addr,len);
	}
	virtual void *malloc(int size,const char *name="")
	{
		return ::malloc(size);
	}
};

#endif

