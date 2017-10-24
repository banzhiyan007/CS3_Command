#include "CmdBase.h"
#include "board.h"
#include <fcs16.hpp>

CMD_0x1F(0x4E)	//NAND READ WRITE COMMAND
{
    static uint8_t *rx_buf=NULL;
    static int      rx_len;
	if(CmdLen < 4)return CMD_CONTINUE;
	static uint16_t len;
	if(CmdLen==4)
	{
        len = Cmd[2]+Cmd[3]*256;
        if(rx_buf)free(rx_buf);
        rx_buf=(uint8_t*)global_malloc(len);
        rx_len=0;
        return CMD_CONTINUE;
    }
    if(CmdLen==5)
    {
        rx_buf[rx_len++]=Cmd[4];
        (*pCmdLen)--;
    }
    if(rx_len<len)return CMD_CONTINUE;
	//if(CmdLen < 4+len)return CMD_CONTINUE;
	if(rx_buf[0]==0x80)			// get nand size
	{
		if(len!=1)return CMD_FINISH;
		uint32_t size;
		uint32_t block_size;
		char buf[11];
		size = 13*1024*1024;//board->spi_flash->block_size*board->spi_flash->block_count;
		block_size = board->spi_flash->block_size;
		buf[0]=rx_buf[0];
		buf[1]=0x08;
		buf[2]=0x00;
		memcpy(&buf[3],&size,4);
		memcpy(&buf[7],&block_size,4);
		tx(buf,3+8);
	}
	else if(rx_buf[0]==0xA0)	// write nand block
	{
		uint32_t block;
		uint16_t crc;
		uint16_t new_crc;
		//Fcs16 fcs;
		memcpy(&block,&rx_buf[1],4);
		memcpy(&crc,&rx_buf[5],2);
		new_crc = crc;//fcs.calc((uint8_t*)&Cmd[11],board->flash->block_size);
		if(crc==new_crc)
		{
			char buf[4];
            board->spi_flash->write_block((uint8_t*)&rx_buf[7],block);
			buf[0]=rx_buf[0];
			buf[1]=0x01;
			buf[2]=0x00;
			buf[3]=0x00;
			tx(buf,4);
		}
		else
		{
			char buf[4];
			buf[0]=rx_buf[0];
			buf[1]=0x01;
			buf[2]=0x00;
			buf[3]=0xFF;
			tx(buf,4);
		}
	}
	else if(rx_buf[0]==0x91)	// read nand block crc
	{
		uint32_t block;
		Fcs16 fcs;
		char buf[5];
		memcpy(&block,&rx_buf[1],4);
		char *pNandData=(char*)malloc(board->spi_flash->block_size);
		board->spi_flash->read_random(block*board->spi_flash->block_size,(uint8_t*)pNandData,board->spi_flash->block_size);
		uint16_t crc = fcs.calc((uint8_t*)pNandData,(int)board->spi_flash->block_size);
		free(pNandData);
		buf[0]=rx_buf[0];
		buf[1]=0x02;
		buf[2]=0x00;
		memcpy(&buf[3],&crc,2);
		tx(buf,3+2);
	}
	else if(rx_buf[0]==0x90)	// read nand block
	{
		Fcs16 fcs;
		uint32_t block;
		uint16_t my_fcs;
		uint16_t len;
		memcpy(&block,&rx_buf[1],4);
		char *pNandData=(char*)malloc(5+board->spi_flash->block_size);
		board->spi_flash->read_random(block*board->spi_flash->block_size,(uint8_t*)pNandData+5,board->spi_flash->block_size);
		my_fcs = fcs.calc((uint8_t*)pNandData+5,board->spi_flash->block_size);
		len = 2+board->spi_flash->block_size;
		pNandData[0]=0x90;
		pNandData[1]=len%256;
		pNandData[2]=len/256;
		memcpy(&pNandData[3],&my_fcs,2);
		tx(pNandData,5+board->spi_flash->block_size);
		free(pNandData);
	}
    free(rx_buf);
    rx_buf=NULL;
	return CMD_FINISH;
}



