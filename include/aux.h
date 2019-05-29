#include "t2fs.h"

int read_MBR(BYTE *buffer);
int superBlock_to_disk(super_block *sb);
int load_superBlock(WORD sector, super_block *sb);
void print_SB(super_block *sb);
int IB_alloc(super_block *sb, BYTE *blk_buffer);
int IB_add_Entry(super_block *sb, BYTE *blk_buffer, fileEntry *entry);
int blk_mem_to_disk(super_block *sb, BYTE *buffer, int block);
int blk_disk_to_mem(super_block *sb, BYTE *buffer, int block);
