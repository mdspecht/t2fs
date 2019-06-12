#include <string.h>
#include "apidisk.h"
#include "mbr.h"
#include "superBlock.h"

static BYTE emptySector[SECTOR_SIZE]= {0};

int clear_partition(PartitionEntry *partition)
{
	DWORD sector;
	memset(emptySector, 0, SECTOR_SIZE);
	for(sector= partition->startSector; sector <= partition->endSector; sector++){
		if(write_sector(sector, emptySector)!=0){
			return -1;
		}
	}
	return 0;
}

int block_disk_to_mem(WORD block, BYTE *buffer)
{
	WORD sectorsPerBlock= SB.getBlockSize()/SECTOR_SIZE;
	WORD sector= SB.getStartSector() + block*sectorsPerBlock;
	BYTE *ptr= buffer;
	int i;
	for(i=0 ;i< sectorsPerBlock;i++){
		if(read_sector (sector, ptr)!=0){
			return -1;
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}
	return 0;
}


int block_mem_to_disk(WORD block, BYTE* buffer)
{
	WORD sectorsPerBlock= SB.getBlockSize()/SECTOR_SIZE;
	WORD sector= SB.getStartSector() + block*sectorsPerBlock;
	BYTE *ptr= buffer;
	int i;
	for(i=0 ;i< sectorsPerBlock;i++){
		if(write_sector (sector, ptr)!=0){
			return -1;
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}
	return 0;
}

