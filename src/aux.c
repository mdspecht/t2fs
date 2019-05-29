#include "t2fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apidisk.h"
#include "debug.h"
#include "aux.h"
#include "bitmap.h"

void print_MBR(Mbr *mbr)
{
	debug_printf("MBR:\n");
	debug_printf("version:\t %04X\n",mbr->version);
	debug_printf("sectorSize:\t %d\n",mbr->sectorSize);
	debug_printf("partitionTableOffet: 0x%04X\n",mbr->partitionTableOffet);
	debug_printf("numPartitions: %d\n",mbr->numPartitions);

	debug_printf("entry[0].startAddr:\t 0x%X\n",mbr->entry[0].startAddr);
	debug_printf("entry[0].endAddr:\t 0x%X\n",mbr->entry[0].endAddr);
	debug_printf("entry[0].name:\t %24s\n",mbr->entry[0].name);
	
	debug_printf("entry[1].startAddr:\t 0x%X\n",mbr->entry[1].startAddr);
	debug_printf("entry[1].endAddr:\t 0x%X\n",mbr->entry[1].endAddr);
	debug_printf("entry[1].name:\t %24s\n",mbr->entry[1].name);
	
	debug_printf("entry[2].startAddr:\t 0x%X\n",mbr->entry[2].startAddr);
	debug_printf("entry[2].endAddr:\t 0x%X\n",mbr->entry[2].endAddr);
	debug_printf("entry[2].name:\t %24s\n",mbr->entry[2].name);
	
	debug_printf("entry[3].startAddr:\t 0x%X\n",mbr->entry[3].startAddr);
	debug_printf("entry[3].endAddr:\t 0x%X\n",mbr->entry[3].endAddr);
	debug_printf("entry[3].name:\t %24s\n",mbr->entry[3].name);

}


void print_SB(super_block *sb)
{
	debug_printf("SUPER BLOCK:\n");
	debug_printf("name : %s\n",          sb->id );
	debug_printf("blockSize : %d\n",     sb->blockSize); 
	debug_printf("startSector : 0x%X\n", sb->startSector);
	debug_printf("numBlocks : %d\n",     sb->numBlocks);

}

int read_MBR(BYTE *buffer){
	Mbr *ptrMBR=(Mbr *)buffer;
	if(read_sector(0, buffer)!=0){
		debug_printf("error at %s\n",__FUNCTION__);
		return -1;
	}
	print_MBR(ptrMBR);
	return 0;
}

int superBlock_to_disk(super_block *sb)
{
	BYTE buffer[SECTOR_SIZE];
	memcpy(buffer, sb, sizeof(*sb));
	write_sector(sb->startSector, buffer);
	return 0;
}

int load_superBlock(WORD sector, super_block *sb)
{
	BYTE buffer[SECTOR_SIZE];
	if(read_sector(sector, buffer)!=0){
		debug_printf("error at %s\n",__FUNCTION__);
		return -1;
	}
	memcpy(sb, buffer, sizeof(*sb));
	return 0;
}


int blk_mem_to_disk(super_block *sb, BYTE *buffer, int block)
{
	WORD sector= sb->startSector + sb->blockSize*block;
	BYTE *ptr= buffer;
	int i;
	for(i=0 ;i< sb->blockSize;i++){
		if(write_sector (sector, ptr)!=0){
			return -1;
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}
	return 0;
}


int blk_disk_to_mem(super_block *sb, BYTE *buffer, int block)
{
	WORD sector= sb->startSector + sb->blockSize*block;
	BYTE *ptr= buffer;
	int i;
	for(i=0 ;i< sb->blockSize;i++){
		if(read_sector (sector, ptr)!=0){
			return -1;
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}
	return 0;
}

int IB_add_Entry(super_block *sb, BYTE *blk_buffer, fileEntry *entry)
{
	int i;
	int max_entries= sb->blockSize*SECTOR_SIZE/sizeof(*entry); 
	fileEntry *entryPtr= (fileEntry *)blk_buffer;
	for(i=0;i< max_entries-1;i++){
		if(entryPtr[i].fileType==FILE_NO_ENTRY){
			memcpy(&entryPtr[i], entry, sizeof(*entry));
			return 0;
		}
	}
	
	//check if last position is a ptr to another IB
	WORD next_IB= *((WORD *)&entryPtr[i]);
	if(next_IB != 0xFFFF){
		//recursively search in linked IB
		BYTE *buffer = malloc(sb->blockSize*SECTOR_SIZE);
		blk_disk_to_mem(sb, buffer, next_IB);
		IB_add_Entry(sb, buffer, entry);
		blk_mem_to_disk(sb, buffer, next_IB);
		free(buffer);
	}else{
		//allocate new IB and finally add the entry
		BYTE *new_IB=NULL;
		int blockNum= IB_alloc(sb, new_IB);
		if(blockNum <0){
			return -1;
		}
		*((WORD *)&entryPtr[i])= blockNum;

		IB_add_Entry(sb, new_IB, entry);
		blk_mem_to_disk(sb, new_IB, blockNum);
		free(new_IB);
	}
	return 0;
}


void reset_IB(super_block*sb, int block)
{

}

int IB_alloc(super_block *sb, BYTE *blk_buffer)
{
	int block= get_free_block();
	if(block==-1){
		return -1;
	}
	set_block_occupied(block);

	blk_buffer= malloc(sb->blockSize*SECTOR_SIZE);
	memset(blk_buffer, 0xFF, sb->blockSize*SECTOR_SIZE);

	return block;
}

