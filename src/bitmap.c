#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apidisk.h"
#include "bitmap.h"

#include "debug.h"
#define debug_printf printf

static int init(struct st_superBlock *sb);
static int load(st_superBlock *sb);
static int set_block_occupied(WORD block);
static void store(void);
static int get_free_block(void);
static int mark_block(WORD block, en_blockState val);
static void print(void);

struct st_bitmap Bitmap={ 	.numBlocks			= 0,	
							.size				= 0,	
                            .occupationBlocks	= 0,
                            .startSector		= 0,
                            .mem				= NULL,
							.init				= init,
							.load				= load,
							.store				= store,
							.getFreeBlock		= get_free_block,
							.markBlock			= mark_block,
							.print				= print
						};


static void print(void)
{
	debug_printf("BITMAP:\n");
	debug_printf("      Numblocks   :%d\n",Bitmap.numBlocks);
	debug_printf("      Size        :%d\n",Bitmap.size);			
	debug_printf("      occupancy   :%d\n",Bitmap.occupationBlocks);	
	debug_printf("      startSector :%d\n",Bitmap.startSector);	
	debug_printf("      mem:\n");
	int i;
	for(i=0;i<Bitmap.size;i++){
		if(i%2==0) debug_printf(" ");
		if(i%16==0) debug_printf("\n%04x: ", i);
		debug_printf("%02x",Bitmap.mem[i]);
	}	
	printf("\n");

}


static int fetchFromSB(struct st_superBlock *sb)
{
	Bitmap.numBlocks= sb->getNumBlocks();
	Bitmap.size= 1 + (sb->getNumBlocks()-1)/8;
	Bitmap.occupationBlocks= 1+(Bitmap.size-1)/(sb->getBlockSize());
	Bitmap.startSector= sb->getStartSector() + sb->getSectorsPerBlock();

	Bitmap.mem= malloc(Bitmap.size);
	if(Bitmap.mem==NULL){
		debug_printf("cant allocate in %s.\n", __FUNCTION__);
		return -1;
	}
	return 0;

}

static int init(struct st_superBlock *sb)
{
	if(fetchFromSB(sb) < 0){
		return -1;
	}

	memset(Bitmap.mem, 0xFF, Bitmap.size);

	//set superblock position to occupied
	set_block_occupied(0);

	WORD b;
	//set numBitmapBlocks to occupied
	for(b=1; b<1+Bitmap.occupationBlocks; b++){
		set_block_occupied(b);
	}
	store();

	return 0;
}


static int load(st_superBlock *sb)
{
	WORD sector;

	if(fetchFromSB(sb) < 0){
		return -1;
	}
 	sector= Bitmap.startSector;

	BYTE buffer[SECTOR_SIZE];
	BYTE *ptr= Bitmap.mem;

	while( (ptr-Bitmap.mem) < Bitmap.size){
		debug_printf("debug read sector: %d.\n", sector);
		if(read_sector (sector, buffer)!=0){
			debug_printf("error trying to read in %s.\n", __FUNCTION__);
			return -1;
		}
		//print_sector(buffer);

		if( (&Bitmap.mem[Bitmap.size] - ptr) > SECTOR_SIZE){
			debug_printf("1: ptr:%p Bitmap.mem:%p", ptr, Bitmap.mem);
			memcpy(ptr, buffer, SECTOR_SIZE);
		}else{
			debug_printf("2: ptr:%p Bitmap.mem:%p\n", ptr, Bitmap.mem);
			debug_printf("2: copySize:%d\n", (&Bitmap.mem[Bitmap.size] - ptr));
			memcpy(ptr, buffer, (&Bitmap.mem[Bitmap.size] - ptr));
		}
		ptr+=SECTOR_SIZE;
		sector++;
	}

#if 0
	int i=0;
	BYTE *dptr;
	dptr= (BYTE*)Bitmap.mem;
	(void)dptr;
	for(i=0;i<Bitmap.size;i++){
		if(i%4==0){
			debug_printf(" 0x");
		}
		debug_printf("%02X",dptr[i]);
	}
#endif
	return 0;
}




//save bitmap memory to disk
static void store(void)
{
	BYTE *ptr= Bitmap.mem;
	WORD sector= Bitmap.startSector;
	BYTE buffer[SECTOR_SIZE] = {0};

	while( (ptr-Bitmap.mem) < Bitmap.size){
		WORD remaining= Bitmap.mem+Bitmap.size-ptr;
		if(remaining < SECTOR_SIZE){
			memset(buffer, 0, SECTOR_SIZE);
			memcpy(buffer, ptr, remaining);
		}else{
			memcpy(buffer, ptr, SECTOR_SIZE);
		}
		write_sector (sector, buffer);
		ptr+=SECTOR_SIZE;
		sector++;
	}
}

static int get_free_block(void)
{
	WORD block;
	BYTE b_free;
	WORD i;
	for(i=0;i< Bitmap.size;i++){
		if(Bitmap.mem[i]!=0x00){
			for(block=0; block<8; block++){
				b_free= (Bitmap.mem[i] >> block) & 0x1;
				if(b_free){
					int ret= 8*i+block;
					mark_block(ret, BLOCK_OCCUPIED);
					return ret;
				}
			}
		}
	}
	return -1;
}

static int out_of_range(WORD block)
{
	return (block >= Bitmap.numBlocks);

}

static int mark_block(WORD block, en_blockState val)
{
	if(out_of_range(block)){
		return -1;
	}

	if(val==BLOCK_FREE){
		Bitmap.mem[block/8] |= 1<<(block%8);

	}else{
		Bitmap.mem[block/8] &= ~(1<<(block%8));
	}
	store();
	return 0;
}

int bitmap_set_block_free(WORD block)
{
	return mark_block(block, BLOCK_FREE);
}

static int set_block_occupied(WORD block)
{
	return mark_block(block, BLOCK_OCCUPIED);
}

