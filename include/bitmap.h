#include "superBlock.h"

typedef enum{BLOCK_OCCUPIED, BLOCK_FREE} en_blockState;

struct st_bitmap
{
	WORD numBlocks;			//numBlocks controled by the bitmap
	WORD size;     			//bitmap size in bytes
	WORD occupationBlocks;	//number of blocks that this bitmap occupies in disk
	WORD startSector;		//startSector of bitmap
	BYTE *mem;				//allocated area that stores the freeBlocks bitmap
	int (*init)(struct st_superBlock *sb);
	int (*load)(struct st_superBlock *sb);
	void (*store)(void);
	int (*getFreeBlock)(void);
	int (*markBlock)(WORD block, en_blockState val);
};

extern struct st_bitmap Bitmap;
