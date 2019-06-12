#ifndef __T2FS_AUX_H__
#define __T2FS_AUX_H__
#include "t2fs.h"
#include  "mbr.h"

#define FILE_REGULAR	1
#define FILE_DIR		2
#define FILE_LINK		3
struct st_opened_dir
{
	char name[MAX_FILE_NAME_SIZE+1];
	int type;
};

struct dataBuffer
{
	BYTE * buffer;
	struct dataBuffer *next;
};

struct fileDescriptor
{
	char name[MAX_FILE_NAME_SIZE+1];
	DWORD size;
	DWORD currFileOffset;
	DWORD currBlockOffset;
	struct dataBuffer *firstDataBlock;
	struct dataBuffer *currDataBlock;

};

typedef WORD dataBlockNumber;

typedef struct st_indexBlock{
	WORD number;
	BYTE *content;
	struct st_indexBlock* next;
}st_indexBlock;



void t2fs_mount(void);
int check_params(stMbr *mbr, WORD sectors_per_block);

#endif



