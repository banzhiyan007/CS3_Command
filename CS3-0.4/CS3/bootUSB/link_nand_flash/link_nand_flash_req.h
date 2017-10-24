#ifndef LINK_NAND_FLASH_REQ_H
#define LINK_NAND_FLASH_REQ_H

extern int link_nand_flash_block_size;
extern int link_nand_flash_block_count;
int link_nand_flash_req_info(void);
int link_nand_flash_req_write(void *data,int len,uint16_t fcs);
int link_nand_flash_req_read_fcs(int block,uint16_t *fcs);

#endif

