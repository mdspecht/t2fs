#include "t2fs.h"
#include "mbr.h"

int block_mem_to_disk(WORD block, BYTE* buffer);
int block_disk_to_mem(WORD block, BYTE *buffer);
int clear_partition(PartitionEntry *partition)
