#include <mbed.h>
#include "version.h"
#include "board.h"
#include "SpiFlash_NOR.h"

Board *board;

static void init_board(void)
{
	board=new Board;
}
INIT_CALL("0",init_board);

#define SPI_MOSI		P1_3
#define SPI_MISO		P1_4
#define SPI_SCK			P1_0
#define SPI_FLASH_CS	P3_5
#define SPI_RAM_CS		P1_1

Board::Board()
{
	mem_region("Board");

	spi_flash=new SpiFlash_NOR(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_FLASH_CS);
	romfs=new ROMFSFileSystem("disk_sys",spi_flash);

	SpiRam *spi_ram=new SpiRam(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_RAM_CS);

	spi_ram->locked = &spi_flash->locked;
	ram=spi_ram;

	mem_region(NULL);
}

REG_FUNCTION(power_reset,NULL,"重启");
REG_FUNCTION(power_off,NULL,"关机");
REG_FUNCTION(reg_value_save,reg_value_save,"保存注册表信息");

VAR_REG_STRING(VERSION_CMD,16,VersionCmd,VERSION_CMD,"版本号");
