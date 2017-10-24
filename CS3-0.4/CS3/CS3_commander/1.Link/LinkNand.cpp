#include <mbed.h>
#include "board.h"
#include "link.h"
#include <Fcs16.hpp>

static void (*save_link_before_tx)(void);
static void (*save_link_after_tx)(void);
static void (*save_link_rx_cmd)(uint8_t cmd,uint8_t *data,uint16_t len);
static void (*save_link_after_rx)(void);

enum
{
	CMD_NAND_READ_INFO=0x80,
	CMD_NAND_WRITE_INFO=0x81,
	CMD_NAND_BLOCK_INDEX=0x82,
	CMD_NAND_WRITE_DATA=0x83,
	CMD_NAND_WRITE_FCS=0x84,
	CMD_NAND_READ_FCS=0x85,
};

static int link_nand_write_info=0;
static int link_nand_block_index=0;
static uint16_t link_nand_fcs=0;
static int link_nand_write_fcs=0;
static Fcs16 *fcs=NULL;
static void link_nand_before_tx(void)
{
	if(link_nand_write_info)
	{
		link_nand_write_info=0;
		uint8_t buf[8];
		memcpy(&buf[0],&board->spi_flash->block_size,4);
		memcpy(&buf[4],&board->spi_flash->block_count,4);
		link_cmd_add(CMD_NAND_WRITE_INFO,buf,8);
	}
	if(link_nand_write_fcs)
	{
		link_nand_write_fcs=0;
		uint8_t buf[2];
		memcpy(&buf[0],&link_nand_fcs,2);
		link_cmd_add(CMD_NAND_WRITE_FCS,buf,2);
	}
	if(save_link_before_tx)save_link_before_tx();
}

static void link_nand_after_tx(void)
{
	if(save_link_after_tx)save_link_after_tx();
}

static void link_nand_rx_cmd(uint8_t cmd,uint8_t *data,uint16_t len)
{
	if(cmd==CMD_NAND_READ_INFO)
	{
		link_nand_write_info=1;
	}
	if(cmd==CMD_NAND_BLOCK_INDEX)
	{
		memcpy(&link_nand_block_index,data,4);
	}
	if(cmd==CMD_NAND_WRITE_DATA)
	{
		board->spi_flash->write_block(data,link_nand_block_index);
		if(fcs==NULL)fcs=new Fcs16;
		link_nand_fcs=fcs->calc(data,len);
		link_nand_write_fcs=1;
	}
	if(cmd==CMD_NAND_READ_FCS)
	{
		memcpy(&link_nand_block_index,data,4);
		uint8_t *buf=(uint8_t *)malloc(board->spi_flash->block_size);
		board->spi_flash->read_block(buf,link_nand_block_index);
		if(fcs==NULL)fcs=new Fcs16;
		link_nand_fcs=fcs->calc(buf,board->spi_flash->block_size);
		free(buf);
		link_nand_write_fcs=1;
	}
	if(save_link_rx_cmd)save_link_rx_cmd(cmd,data,len);
}

static void link_nand_after_rx(void)
{
	if(save_link_after_rx)save_link_after_rx();
}

static void init_link_nand(void)
{
	save_link_before_tx=link_before_tx;
	save_link_after_tx=link_after_tx;
	save_link_rx_cmd=link_rx_cmd;
	save_link_after_rx=link_after_rx;

	link_before_tx=link_nand_before_tx;
	link_after_tx=link_nand_after_tx;
	link_rx_cmd=link_nand_rx_cmd;
	link_after_rx=link_nand_after_rx;
}

INIT_CALL("1",init_link_nand);

