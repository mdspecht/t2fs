#ifndef __SUPER_BLOCK_H__
#define __SUPER_BLOCK_H__
#include "t2fs.h"
#include "mbr.h"


typedef struct st_superBlock{
	int  (*load)(WORD sector);
	void  (*init)(PartitionEntry *partition, int sectorsPerBlock);
	int   (*store)(void);
	char* (*getName)(void);
	WORD  (*getStartSector)(void);
	WORD  (*getSectorsPerBlock)(void);
	WORD  (*getBlockSize)(void);
	WORD  (*getNumBlocks)(void);
	WORD  (*getRootIndexBlock)(void);
	void  (*setRootIndexBlock)(WORD block);
} st_superBlock;

extern st_superBlock SB;

void print_SB(void);

#endif
