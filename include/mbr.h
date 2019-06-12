#ifndef __MBR_H__
#define __MBR_H__

#include "t2fs.h"

#define MAX_PARTITION_NAME_SIZE 24
#pragma pack (push,1)
typedef struct{
	DWORD startSector;
	DWORD endSector;
	char name[MAX_PARTITION_NAME_SIZE];
} PartitionEntry;

typedef struct {
	WORD version;
	WORD sectorSize;
	WORD partitionTableOffet;
	WORD numPartitions;
	PartitionEntry entry[4];
} stMbr;
#pragma pack (pop)

extern stMbr MBR;
int block_mem_to_disk(WORD block, BYTE* buffer);
int block_disk_to_mem(WORD block, BYTE *buffer);
int clear_partition(PartitionEntry *partition);
int load_MBR(void);


#endif
