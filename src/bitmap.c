#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "apidisk.h"
#include "debug.h"

BYTE *Bitmap= NULL;
WORD BitmapSize=0;
DWORD numBlocks;


void print_sector(BYTE* buffer)
{
	int i;
	for(i=0;i<SECTOR_SIZE;i++){
		debug_printf("0x%0X ", buffer[i]);
	}
	debug_printf("\n");
}

int load_bitmap(super_block *sb)
{
	BYTE buffer[SECTOR_SIZE];
	WORD sector= sb->startSector + sb->blockSize;
	BitmapSize= sb->numBlocks/8;
	numBlocks= sb->numBlocks;
	Bitmap= malloc(BitmapSize);
	BYTE *ptr= Bitmap;
	if(Bitmap==NULL){
		return -1;
	}
	while( (ptr-Bitmap) < BitmapSize){
		debug_printf("debug read sector: %d.\n", sector);
		if(read_sector (sector, buffer)!=0){
			debug_printf("error trying to read in %s.\n", __FUNCTION__);
			return -1;
		}
		print_sector(buffer);

		if(  (&Bitmap[BitmapSize] - ptr) > SECTOR_SIZE){
			debug_printf("1: ptr:%p Bitmap:%p", ptr, Bitmap);
			memcpy(ptr, buffer, SECTOR_SIZE);
		}else{
			debug_printf("2: ptr:%p Bitmap:%p", ptr, Bitmap);
			memcpy(ptr, buffer, (&Bitmap[BitmapSize] - ptr));
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}

	int i=0;
	BYTE *dptr;
	dptr= (BYTE*)Bitmap;
	(void)dptr;
	for(i=0;i<BitmapSize;i++){
		if(i%4==0){
			debug_printf(" 0x");
		}
		debug_printf("%02X",dptr[i]);
	}
	
	return 0;
}


int bitmapInit(super_block *sb)
{
	numBlocks= sb->numBlocks;	
	BitmapSize= sb->numBlocks/8;
	Bitmap= malloc(BitmapSize);
	if(Bitmap==NULL){
		debug_printf("cant allocate in %s.\n", __FUNCTION__);
		return -1;
	}
	memset(Bitmap, 0xFF, BitmapSize);
	WORD numBitmapBlocks= 1+(BitmapSize-1)/(SECTOR_SIZE*sb->blockSize);

	//set superblock position to occupied
	set_block_occupied(0);

	WORD b;
	//set numBitmapBlocks to occupied
	for(b=1;b<1+numBitmapBlocks;b++){
		set_block_occupied(b);
	}
	bitmap_flush(sb);

	return 0;
}

//save bitmap memory to disk
void bitmap_flush(super_block *sb){
	BYTE *ptr= Bitmap;
	WORD sector= sb->startSector + sb->blockSize;
	while( (ptr-Bitmap) < BitmapSize){
		write_sector (sector, ptr);
		ptr+=SECTOR_SIZE;
		sector++;
	}
}

int get_free_block(void)
{
	WORD block;
	BYTE b_free;
	WORD i;
	for(i=0;i< BitmapSize;i++){
		if(Bitmap[i]!=0x00){
			for(block=0; block<8; block++){
				b_free= (Bitmap[i] >> block) & 0x1;
				if(b_free){
					return 8*i+ block;
				}
			}
		}
	}
	return -1;
}

static int out_of_range(WORD block){
	return (block >= numBlocks);

}

static int mark_block(WORD block, en_blockState val){
	if(out_of_range(block)){
		return -1;
	}

	if(val==BLOCK_FREE){
		Bitmap[block/8] |= 1<<(block%8);

	}else{
		Bitmap[block/8] &= ~(1<<(block%8));
	}
	return 0;
}

int set_block_free(WORD block){
	return mark_block(block, BLOCK_FREE);
}

int set_block_occupied(WORD block){
	return mark_block(block, BLOCK_OCCUPIED);
}


