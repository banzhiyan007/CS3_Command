#include <mbed.h>
#include "link.h"

static void (*save_link_before_tx)(void);
static void (*save_link_after_tx)(void);
static void (*save_link_rx_cmd)(uint8_t cmd,uint8_t *data,uint16_t len);
static void (*save_link_after_rx)(void);

enum
{
	CMD_READ_REG_TABLE=0x10,
	CMD_WRITE_REG_TABLE=0x11,
	CMD_WRITE_REG_LOCAL=0x12,
	CMD_WRITE_REG_REMOTE=0x13,

	CMD_REPORT_FIFO_FREE=0x20,
	CMD_WRITE_FIFO_DATA=0x21,
};

struct LINK_REG
{
	uint8_t total_size;
	uint8_t changed;
	uint8_t type;
	uint8_t data_size;
	void   *data_ptr;
	uint16_t link;
	uint8_t name_size;
	uint8_t info_size;
	uint8_t index;
	uint8_t data_name_info[0];
}__attribute__((packed));

static int		regLocalTableSize=0;
static uint8_t *regLocalTable=NULL;
static int		regLocalCount=0;
LINK_REG	  **linkRegLocal=NULL;

static int		regRemoteTableSize=0;
static uint8_t *regRemoteTable=NULL;
int		regRemoteCount=0;
LINK_REG	  **linkRegRemote=NULL;

static void link_reg_local_build(void)
{
	regLocalCount=reg_count();
	regLocalTableSize=0;
	for(int i=0;i<regLocalCount;i++)
	{
		char *name=(char*)reg_name(i);
		char *info=(char*)reg_info(i);
		regLocalTableSize+=sizeof(LINK_REG)+reg_size(i)+strlen(name)+1+strlen(info)+1;
	}
	linkRegLocal=(LINK_REG**)malloc(regLocalCount*sizeof(LINK_REG*));
	regLocalTable=(uint8_t*)malloc(regLocalTableSize);
	memset(regLocalTable,0,regLocalTableSize);

	uint8_t *buf=regLocalTable;
	for(int i=0;i<regLocalCount;i++)
	{
		char *name=(char*)reg_name(i);
		char *info=(char*)reg_info(i);
		linkRegLocal[i]=(LINK_REG*)buf;
		linkRegLocal[i]->total_size=sizeof(LINK_REG)+reg_size(i)+strlen(name)+1+strlen(info)+1;
		linkRegLocal[i]->changed=0;
		linkRegLocal[i]->type=reg_type(i);
		linkRegLocal[i]->data_ptr=reg_get_data_ptr(i);
		linkRegLocal[i]->data_size=reg_size(i);
		linkRegLocal[i]->name_size=strlen(name)+1;
		linkRegLocal[i]->info_size=strlen(info)+1;
		linkRegLocal[i]->link=0;
		linkRegLocal[i]->index=i;
		if(linkRegLocal[i]->type==REG_TYPE_FUNCTION)
		{
			linkRegLocal[i]->data_ptr=NULL;
		}
		if(linkRegLocal[i]->data_ptr!=NULL)memcpy(linkRegLocal[i]->data_name_info,linkRegLocal[i]->data_ptr,linkRegLocal[i]->data_size);
		memcpy(&linkRegLocal[i]->data_name_info[linkRegLocal[i]->data_size],name,strlen(name)+1);
		memcpy(&linkRegLocal[i]->data_name_info[linkRegLocal[i]->data_size+linkRegLocal[i]->name_size],info,strlen(info)+1);
		buf+=linkRegLocal[i]->total_size;
	}
}

static void link_reg_remote_build(uint8_t *data,int len)
{
	if(regRemoteTable!=NULL)return;
	regRemoteTableSize=len;
	regRemoteTable=(uint8_t *)malloc(regRemoteTableSize);
	memcpy(regRemoteTable,data,len);

	int remoteCount=0;
	LINK_REG *reg;
	reg=(LINK_REG *)regRemoteTable;
	while(((uint32_t)reg)<(((uint32_t)regRemoteTable)+len))
	{
		reg=(LINK_REG *)(((uint32_t)reg)+reg->total_size);
		remoteCount++;
	}
	linkRegRemote=(LINK_REG**)malloc(remoteCount*sizeof(LINK_REG*));

	reg=(LINK_REG *)regRemoteTable;
	for(int i=0;i<remoteCount;i++)
	{
		linkRegRemote[i]=reg;
		linkRegRemote[i]->link=0;
		for(int j=0;j<regLocalCount;j++)
		{
			if(strcmp((char*)&linkRegRemote[i]->data_name_info[linkRegRemote[i]->data_size],(char*)&linkRegLocal[j]->data_name_info[linkRegLocal[j]->data_size])==0)
			{
				linkRegRemote[i]->link=j+1;
				break;
			}
		}
		reg=(LINK_REG *)(((uint32_t)reg)+reg->total_size);
	}
	regRemoteCount=remoteCount;
}

static int need_write_reg_table=0;
static int write_reg_table_successed=0;
static void link_reg_before_tx(void)
{
	if(regRemoteTable==NULL)
	{
		link_cmd_add(CMD_READ_REG_TABLE,NULL,0);
	}
	if(need_write_reg_table)
	{
		need_write_reg_table=0;
		link_cmd_add(CMD_WRITE_REG_TABLE,regLocalTable,regLocalTableSize);
		write_reg_table_successed=1;
	}
	if(write_reg_table_successed==2)
	{
		for(int i=0;i<regLocalCount;i++)
		{
			LINK_REG *reg=linkRegLocal[i];
			if(reg->data_ptr==NULL)continue;
			if(reg->type==REG_TYPE_BUFFER)continue;
			if(reg->type==REG_TYPE_FUNCTION&&reg->changed==0)continue;
			if(memcmp(reg->data_name_info,reg->data_ptr,reg->data_size)!=0||reg->changed)
			{
				memcpy(reg->data_name_info,reg->data_ptr,reg->data_size);
				link_cmd_add(CMD_WRITE_REG_LOCAL,&reg->index,1+reg->data_size);
				reg->changed=0;
			}
		}
		for(int i=0;i<regLocalCount;i++)
		{
			LINK_REG *reg=linkRegLocal[i];
			if(reg->type!=REG_TYPE_BUFFER)continue;
			if(strcmp((char*)&reg->data_name_info[reg->data_size+reg->name_size],"RX")==0)
			{
				Fifo *fifo=*((Fifo**)reg->data_ptr);
				if(fifo!=NULL)
				{
					uint32_t free_length=fifo->free_length();
					if(free_length>0xFFFF)free_length=0xFFFF;
					char buf[3];
					buf[0]=reg->index;
					memcpy(&buf[1],&free_length,2);
					link_cmd_add(CMD_REPORT_FIFO_FREE,buf,3);
				}
			}
			if(strcmp((char*)&reg->data_name_info[reg->data_size+reg->name_size],"TX")==0)
			{
				Fifo *fifo=*((Fifo**)reg->data_ptr);
				if(fifo!=NULL)
				{
					uint16_t free_length=reg->link;
					uint16_t frame_free=LINK_FRAME_SIZE-(linkFrameHead->size+3+1+4);
					uint32_t data_lenght=fifo->length();
					if(data_lenght>0xFFFF)data_lenght=0xFFFF;
					uint16_t tx_len=free_length;
					if(tx_len>frame_free)tx_len=frame_free;
					if(tx_len>data_lenght)tx_len=data_lenght;

					if(tx_len>0)
					{
						uint8_t *ptr=linkFrame+linkFrameHead->size;
						ptr[0]=CMD_WRITE_FIFO_DATA;
						ptr[1]=(tx_len+1)%256;
						ptr[2]=(tx_len+1)/256;
						ptr[3]=reg->index;
						fifo->read(&ptr[4],tx_len);
						linkFrameHead->size+=3+1+tx_len;
					}
					reg->link=0;
				}
			}
		}
	}
	if(regRemoteTable!=NULL)
	{
		for(int i=0;i<regRemoteCount;i++)
		{
			LINK_REG *reg=linkRegRemote[i];
			if(reg->changed)
			{
				if(reg->type==REG_TYPE_BUFFER)continue;
				link_cmd_add(CMD_WRITE_REG_REMOTE,&reg->index,1+reg->data_size);
				reg->changed=0;
			}
		}
	}
	if(save_link_before_tx)save_link_before_tx();
}

static void link_reg_after_tx(void)
{
	if(save_link_after_tx)save_link_after_tx();
}

static void link_reg_rx_cmd(uint8_t cmd,uint8_t *data,uint16_t len)
{
	if(cmd==CMD_READ_REG_TABLE)
	{
		need_write_reg_table=1;
	}
	if(cmd==CMD_WRITE_REG_TABLE)
	{
		link_reg_remote_build(data,len);
	}
	if(cmd==CMD_WRITE_REG_LOCAL)
	{
		uint8_t index=data[0];
		LINK_REG *reg=linkRegRemote[index];
		memcpy(reg->data_name_info,data+1,reg->data_size);
		uint8_t link=reg->link;
		if(link!=0)
		{
			link=link-1;
			if(linkRegLocal[link]->data_ptr!=NULL&&linkRegLocal[link]->type!=REG_TYPE_FUNCTION)
			{
				memcpy(linkRegLocal[link]->data_ptr,linkRegRemote[index]->data_name_info,linkRegRemote[index]->data_size);
				memcpy(linkRegLocal[link]->data_name_info,linkRegRemote[index]->data_name_info,linkRegRemote[index]->data_size);
			}
		}
	}
	if(cmd==CMD_WRITE_REG_REMOTE)
	{
		int index=data[0];
		if(linkRegLocal[index]->type==REG_TYPE_FUNCTION)
		{
			reg_exec_function_by_index(index);
		}
		else
		{
			memcpy(linkRegLocal[index]->data_name_info,data+1,linkRegLocal[index]->data_size);
			if(linkRegLocal[index]->data_ptr!=NULL)memcpy(linkRegLocal[index]->data_ptr,data+1,len-1);
		}
	}
	if(cmd==CMD_REPORT_FIFO_FREE)
	{
		int index=data[0];
		uint16_t free_length;
		memcpy(&free_length,&data[1],2);
		LINK_REG *reg=linkRegRemote[index];
		if(reg->link!=0)
		{
			linkRegLocal[reg->link-1]->link=free_length;
		}
	}
	if(cmd==CMD_WRITE_FIFO_DATA)
	{
		int index=data[0];
		LINK_REG *reg=linkRegRemote[index];
		if(reg->link!=0)
		{
			Fifo *fifo=*((Fifo**)linkRegLocal[reg->link-1]->data_ptr);
			if(fifo!=NULL)fifo->write(&data[1],len-1);
		}
	}
	if(save_link_rx_cmd)save_link_rx_cmd(cmd,data,len);
}

static void link_reg_after_rx(void)
{
	if(write_reg_table_successed==1)
	{
		write_reg_table_successed=2;
		for(int i=0;i<regLocalCount;i++)
		{
			if(linkRegLocal[i]->type==REG_TYPE_FUNCTION)continue;
			linkRegLocal[i]->changed=1;
		}
	}
	if(save_link_after_rx)save_link_after_rx();
}

static void init_link_reg(void)
{
	link_reg_local_build();

	save_link_before_tx=link_before_tx;
	save_link_after_tx=link_after_tx;
	save_link_rx_cmd=link_rx_cmd;
	save_link_after_rx=link_after_rx;

	link_before_tx=link_reg_before_tx;
	link_after_tx=link_reg_after_tx;
	link_rx_cmd=link_reg_rx_cmd;
	link_after_rx=link_reg_after_rx;
}

INIT_CALL("5",init_link_reg);

//-------------------------------------------------------------------------------------------------
uint32_t link_reg_count(void)
{
	return regLocalCount+regRemoteCount;
}
//-------------------------------------------------------------------------------------------------
const char *link_reg_name(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return (char*)&reg->data_name_info[reg->data_size];
}
//-------------------------------------------------------------------------------------------------
uint32_t link_reg_type(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return reg->type;
}
//-------------------------------------------------------------------------------------------------
uint32_t link_reg_size(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return reg->data_size;
}
//-------------------------------------------------------------------------------------------------
const char *link_reg_info(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return (char*)&reg->data_name_info[reg->data_size+reg->name_size];
}
//-------------------------------------------------------------------------------------------------
int link_reg_index(const char *name)
{
	for(int i=0;i<regLocalCount;i++)
	{
		if(strcmp((char*)&linkRegLocal[i]->data_name_info[linkRegLocal[i]->data_size],name)==0)return i;
	}
	for(int i=0;i<regRemoteCount;i++)
	{
		if(strcmp((char*)&linkRegRemote[i]->data_name_info[linkRegRemote[i]->data_size],name)==0)return regLocalCount+i;
	}
	return -1;
}
//-------------------------------------------------------------------------------------------------
uint32_t link_reg_get_u32_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	uint32_t r;
	memcpy(&r,reg->data_name_info,reg->data_size);
	return r;
}
//-------------------------------------------------------------------------------------------------
char *link_reg_get_string_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return (char*)reg->data_name_info;
}
//-------------------------------------------------------------------------------------------------
unsigned char *link_reg_get_bytes_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return (unsigned char*)reg->data_name_info;
}
//-------------------------------------------------------------------------------------------------
unsigned char *link_reg_get_buffer_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	return (unsigned char*)reg->data_name_info;
}
//-------------------------------------------------------------------------------------------------
float link_reg_get_float_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	float r;
	memcpy(&r,reg->data_name_info,reg->data_size);
	return r;
}
//-------------------------------------------------------------------------------------------------
double link_reg_get_double_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	double r;
	memcpy(&r,reg->data_name_info,reg->data_size);
	return r;
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_u32_by_index(int index,uint32_t value)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		memcpy(reg->data_ptr,&value,reg->data_size);
	}
	memcpy(reg->data_name_info,&value,reg->data_size);
	reg->changed=1;
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_string_by_index(int index,char *pValue)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		strcpy((char*)reg->data_ptr,pValue);
	}
	strcpy((char*)reg->data_name_info,pValue);
	reg->changed=1;
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_bytes_by_index(int index,uint8_t *pValue,int len)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		memcpy(reg->data_ptr,pValue,len);
	}
	memcpy(reg->data_name_info,pValue,len);
	reg->changed=1;
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_float_by_index(int index,float value)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		memcpy(reg->data_ptr,&value,reg->data_size);
	}
	memcpy(reg->data_name_info,&value,reg->data_size);
	reg->changed=1;
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_double_by_index(int index,double value)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		memcpy(reg->data_ptr,&value,reg->data_size);
	}
	memcpy(reg->data_name_info,&value,reg->data_size);
	reg->changed=1;
}
//-------------------------------------------------------------------------------------------------
void link_reg_exec_function_by_index(int index)
{
	LINK_REG *reg=(index<regLocalCount)?linkRegLocal[index]:linkRegRemote[index-regLocalCount];
	if(index<regLocalCount)
	{
		for(int i=0;i<regRemoteCount;i++)
		{
			if(strcmp((char*)&linkRegRemote[i]->data_name_info[linkRegRemote[i]->data_size],(char*)&linkRegLocal[index]->data_name_info[linkRegLocal[index]->data_size])==0)
			{
				linkRegRemote[i]->changed=1;
				wait_ms(50);
				break;
			}
		}
		reg_exec_function_by_index(index);
	}
	else
	{
		reg->changed=1;
	}
}
//-------------------------------------------------------------------------------------------------
uint32_t link_reg_get_u32(const char *name,uint32_t default_value)
{
	int index=link_reg_index(name);
	if(index<0)return default_value;
	return link_reg_get_u32_by_index(index);
}
//-------------------------------------------------------------------------------------------------
char *link_reg_get_string(const char *name,const char *default_value)
{
	int index=link_reg_index(name);
	if(index<0)return (char*)default_value;
	return link_reg_get_string_by_index(index);
}
//-------------------------------------------------------------------------------------------------
unsigned char *link_reg_get_bytes(const char *name,uint8_t *default_value)
{
	int index=link_reg_index(name);
	if(index<0)return (unsigned char*)default_value;
	return link_reg_get_bytes_by_index(index);
}
//-------------------------------------------------------------------------------------------------
unsigned char *link_reg_get_buffer(const char *name,uint8_t *default_value)
{
	int index=link_reg_index(name);
	if(index<0)return (unsigned char*)default_value;
	return link_reg_get_buffer_by_index(index);
}
//-------------------------------------------------------------------------------------------------
float link_reg_get_float(const char *name,float default_value)
{
	int index=link_reg_index(name);
	if(index<0)return default_value;
	return link_reg_get_float_by_index(index);
}
//-------------------------------------------------------------------------------------------------
double link_reg_get_double(const char *name,double default_value)
{
	int index=link_reg_index(name);
	if(index<0)return default_value;
	return link_reg_get_double_by_index(index);
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_u32(const char *name,uint32_t value)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_set_u32_by_index(index,value);
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_string(const char *name,char *pValue)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_set_string_by_index(index,pValue);
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_bytes(const char *name,uint8_t *pValue,int len)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_set_bytes_by_index(index,pValue,len);
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_float(const char *name,float value)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_set_float_by_index(index,value);
}
//-------------------------------------------------------------------------------------------------
void link_reg_set_double(const char *name,double value)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_set_double_by_index(index,value);
}
//-------------------------------------------------------------------------------------------------
void link_reg_exec_function(const char *name)
{
	int index=link_reg_index(name);
	if(index<0)return;
	return link_reg_exec_function_by_index(index);
}
//-------------------------------------------------------------------------------------------------
