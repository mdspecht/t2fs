#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "superBlock.h"
#include "apidisk.h"

#include "debug.h"

#define debug_printf printf

#pragma pack(push,1)

typedef union{
	struct{
		char name[MAX_PARTITION_NAME_SIZE];
		WORD startSector;
		WORD sectorsPerBlock;
		WORD blockSize;
		WORD numBlocks;
		WORD rootIndexBlock;
	}attr;
	BYTE mem[SECTOR_SIZE];
}un_superBlock_data;

#pragma pack(pop)

static un_superBlock_data _SB;

static int load(WORD sector);
static void init(PartitionEntry *partition, int sectorsPerBlock);
static int store(void);
static char *getName(void) { return _SB.attr.name; }
static WORD getStartSector(void) { return _SB.attr.startSector; }
static WORD getSectorsPerBlock(void) { return _SB.attr.sectorsPerBlock; }
static WORD getBlockSize(void) { return _SB.attr.blockSize; }
static WORD getNumBlocks(void) { return _SB.attr.numBlocks; }
static WORD getRootIndexBlock(void) { return _SB.attr.rootIndexBlock; }
static void setRootIndexBlock(WORD block) { _SB.attr.rootIndexBlock= block; }


st_superBlock SB= {.load 				= load,
				   .init 				= init,
				   .store				= store,
				   .getName            	= getName,     
				   .getStartSector     	= getStartSector,
				   .getSectorsPerBlock 	= getSectorsPerBlock,
				   .getBlockSize       	= getBlockSize,
				   .getNumBlocks       	= getNumBlocks,
				   .getRootIndexBlock  	= getRootIndexBlock,
				   .setRootIndexBlock  	= setRootIndexBlock};

void print_SB(void)
{
	debug_printf("SUPER BLOCK:\n");
	debug_printf("name          : %s\n",   _SB.attr.name );
	debug_printf("startSector   : 0x%X\n", _SB.attr.startSector);
	debug_printf("sectorPerBlock: 0x%X\n", _SB.attr.sectorsPerBlock);
	debug_printf("blockSize     : %d\n",   _SB.attr.blockSize); 
	debug_printf("numBlocks     : %d\n",   _SB.attr.numBlocks);
	debug_printf("rootIndexBlock: %d\n",   _SB.attr.rootIndexBlock);

}

static int load(WORD sector)
{
	if(read_sector(sector, _SB.mem)!=0){
		debug_printf("error at %s\n",__FUNCTION__);
		return -1;
	}
	return 0;
}

static int store(void)
{
	if(write_sector(_SB.attr.startSector, _SB.mem)!=0){
		return -1;
	}
	return 0;
}

static void init(PartitionEntry *partition, int sectorsPerBlock)
{
	strncpy(_SB.attr.name, partition->name, MAX_FILE_NAME_SIZE);
	_SB.attr.startSector= partition->startSector;
	_SB.attr.sectorsPerBlock= sectorsPerBlock;
	_SB.attr.blockSize= sectorsPerBlock*SECTOR_SIZE;
	_SB.attr.numBlocks= 1+ (partition->endSector - partition->startSector)/sectorsPerBlock;

	store();


}
















