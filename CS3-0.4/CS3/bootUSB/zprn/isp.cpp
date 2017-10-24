#include <mbed.h>
#include <RawUSBPrinter.h>
#include <SpiFlash_W25Q64BV.h>
#include <beeper.hpp>
#include <Fcs16.hpp>
#include "link_nand_flash_req.h"
   
extern void lcd_clear(void);
extern void lcd_update(void);
extern void lcd_puts(int x,int y,const char *str);
extern void lcd_put_int(int x,int y,uint32_t n);
extern void lcd_display_isp(void);
extern void lcd_display_need_power_off(void);
extern void lcd_display_updating(void);
extern void lcd_display_updating_error(void);

//int RawUSBPrinterFifoRxSize=5*1024;
//int USB_DOUBLE_BUFFER=0;
RawUSBPrinter *usb;
static void init_usb(void)
{
	usb=new RawUSBPrinter();
	delete usb->fifo_rx;
	usb->fifo_rx=new Fifo(5*1024);
}
INIT_CALL("0",init_usb);

extern Beeper beeper;

//-------------------------------------------------------------------------------------------------
extern void frame_write(uint8_t *buf,int len);
extern void frame_send(unsigned char type,uint8_t *buf,unsigned short len);
extern int  frame_wait(unsigned char *type,void *buf,unsigned short *len,int max_size,int timeout);
//-------------------------------------------------------------------------------------------------
#define FRAME_FLASH         0x01
#define FRAME_NAND          0x02

#define CMD_HANDSHAKE		0xA1
#define CMD_SET_BAUDRATE	0xA3
#define CMD_GET_CONFIG		0xA5
#define CMD_GET_CRC			0xA7
#define CMD_WRITE_MEM		0xA9
#define CMD_RESET			0xAA

#define CMD_EXT_CPU			0xB1

__attribute__((aligned(4)))
static uint8_t data[4096+16];
void isp_loop(void)
{
    Fcs16 fcs;
	Timer timeout_timer;

    timeout_timer.start();

	while(1)
	{
		unsigned char cmd;
		unsigned short len;
		int frame_type;

        frame_type = frame_wait(&cmd, data, &len, 16+4096, 1000);
		if(frame_type==FRAME_FLASH)
		{
			timeout_timer.reset();
			if(cmd == CMD_EXT_CPU)
			{
				void isp_ext_cpu(int cpu_id);
				isp_ext_cpu(data[0]);
			}
			else if(cmd == CMD_HANDSHAKE)
			{
				if(len != 0) { break; }
				frame_send(cmd, 0, 0);
			}
			else if(cmd == CMD_GET_CONFIG)
			{
				if(len != 0) { break; }
                struct
                {
                    unsigned long _DEFAULT_BASE;
                    unsigned long _FLASH_SIZE;
                    unsigned long _BLOCK_SIZE;
                }CONFIG;
                CONFIG._DEFAULT_BASE = 0;
                CONFIG._FLASH_SIZE = isp_max_size();
                CONFIG._BLOCK_SIZE = isp_block_size();
				frame_send(cmd, (uint8_t *)&CONFIG, sizeof(CONFIG));
				beeper.beep(3000,100);
			}
			else if(cmd == CMD_GET_CRC)
			{	
				unsigned long from, size=0;
				unsigned short crc;
				if(len != 6) { break; }
				memcpy(&from, data, 4);
				memcpy(&size, data + 4, 2);
				if(isp_read_block(from/isp_block_size(), (uint32_t*)(data + 4), size))
				{	
					crc = 0x1234;
					frame_send(cmd, (uint8_t*)&crc, 2);
				}
				else
				{
                    crc = fcs.calc(data+4, size);
                    frame_send(cmd, (uint8_t*)&crc, 2);
				}
			}	
			else if(cmd == CMD_WRITE_MEM)
			{
				unsigned long from, size;
				unsigned short crc;
				if(len <= 4) { break; }
				memcpy(&from, data, 4);
				size = len - 4;
				crc = fcs.calc(data + 4, size);
				if(isp_write_block(from/isp_block_size(), (uint32_t*)(data + 4), size))
				{
					crc = 0x1234;
					frame_send(cmd, (uint8_t*)&crc, 2);
				}
				else
				{
                    frame_send(cmd, (uint8_t*)&crc, 2);
				}
			}
			else if(cmd == CMD_RESET)
			{
				beeper.beep(3000,100);
				cpu_reset();
			}
		}
		else if(frame_type==FRAME_NAND)
		{
			timeout_timer.reset();
			cmd = data[0];
			if(cmd == 0x80)     // get nand size
			{
				if(len!=1)break;
				uint32_t size;
				uint32_t block_size;
				char buf[11];
				link_nand_flash_req_info();
				size = link_nand_flash_block_size*link_nand_flash_block_count;
				block_size = link_nand_flash_block_size;
				buf[0]=cmd;
				buf[1]=0x08;
				buf[2]=0x00;
				memcpy(&buf[3],&size,4);
				memcpy(&buf[7],&block_size,4);
				frame_write((uint8_t*)buf,3+8);
			}
			else if(cmd==0xA0)	// write nand block
			{
				static int count=0;
				if(count==0)
				{
					lcd_clear();
					lcd_puts(0,0,"NAND WRITE");
					lcd_update();
					count++;
				}
				else
				{
					if(count%10==0)
					{
						lcd_put_int(0,16,count);
						lcd_update();
					}
					count++;
				}

				uint32_t block;
				uint16_t rcv_fcs;
				uint16_t new_fcs;
				memcpy(&block,&data[1],4);
				memcpy(&rcv_fcs,&data[5],2);
				new_fcs=fcs.calc(&data[7],len-7);
				if(rcv_fcs==new_fcs)
				{
					char buf[4];
					if(link_nand_flash_req_write(&data[7],block,new_fcs))
					{
						buf[0]=cmd;
						buf[1]=0x01;
						buf[2]=0x00;
						buf[3]=0x00;
						frame_write((uint8_t*)buf,4);
					}
					else
					{
						char buf[4];
						buf[0]=cmd;
						buf[1]=0x01;
						buf[2]=0x00;
						buf[3]=0xFF;
						frame_write((uint8_t*)buf,4);
					}
				}
				else
				{
					char buf[4];
					buf[0]=cmd;
					buf[1]=0x01;
					buf[2]=0x00;
					buf[3]=0xFF;
					frame_write((uint8_t*)buf,4);
				}
			}
			else if(cmd==0x91)	// read nand block crc
			{
				static int count=0;
				if(count==0)
				{
					lcd_clear();
					lcd_puts(0,0,"NAND CRC");
					lcd_update();
					count++;
				}
				else
				{
					if(count%10==0)
					{
						lcd_put_int(0,16,count);
						lcd_update();
					}
					count++;
				}
				
				uint32_t block;
				uint16_t my_fcs;
				char buf[5];
				memcpy(&block,&data[1],4);
				link_nand_flash_req_read_fcs(block,&my_fcs);
				buf[0]=cmd;
				buf[1]=0x02;
				buf[2]=0x00;
				memcpy(&buf[3],&my_fcs,2);
				frame_write((uint8_t*)buf,3+2);
			}
			else if(cmd==0x90)	// read nand block
			{
				uint32_t block;
				uint16_t my_fcs;
				uint16_t len;
				memcpy(&block,&data[1],4);
				//nand.read_block(data+5,block);
				my_fcs = 0;//fcs.calc(data+5,nand.block_size);
				len = 2+0;//nand.block_size;
				data[0]=cmd;
				data[1]=len%256;
				data[2]=len/256;
				memcpy(&data[3],&my_fcs,2);
				frame_write((uint8_t*)data,5+0);//nand.block_size);
			}
			else if(cmd==0xF0)	// finish
			{
				lcd_clear();
				lcd_puts(0,8,(char*)&data[1]);
				lcd_update();
				usb->disconnect();
				beeper.beep(3000,1000);
			}
		}
		else
		{
			if(0)//timeout_timer.read_ms()>60000)
			{
				beeper.beep(3000,100);
				cpu_reset();
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------
void isp(void)
{
	lcd_display_isp();

	beeper.beep(3000,100);
	wait_ms(150);
	beeper.beep(3000,100);

	isp_loop();
    while(1);
}
