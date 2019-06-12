#ifndef __DIR_H__
#define __DIR_H__

#include "superBlock.h"

#pragma pack(push, 1)
typedef struct{
	DIRENT2 attr;
	WORD index_block;
} dirRecordDisk;
#pragma pack(pop)

typedef struct dirRecordNode{
	dirRecordDisk entry;
	struct dirRecordNode *next;
}dirRecordNode;

typedef struct dirDescriptor{
	char name[MAX_FILE_NAME_SIZE+1];
//	st_indexBlock *indexBlock;
//	BYTE *dataBuffer;
	WORD indexBlock;  
	DWORD numRecords;
	DWORD currRecordOffset;
	dirRecordNode *firstRecord;
	dirRecordNode *currRecord;	
} dirDescriptor;

extern dirDescriptor RootDescriptor;

int root_init(st_superBlock *sb);
void root_load(void);
WORD dir_count_entries(BYTE *indexBlockBuffer);

#endif
