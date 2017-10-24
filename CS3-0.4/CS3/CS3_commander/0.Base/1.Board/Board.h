#ifndef BOARD_H
#define BOARD_H

#include <SpiFlash_W25Q64BV.h>
#include "SpiRam.hpp"
#include <ROMFSFileSystem.h>

class Board
{
public:
	Ram		 *ram;
	SpiFlash *spi_flash;
	ROMFSFileSystem *romfs;
public:
	Board();
};
extern Board *board;

#endif
