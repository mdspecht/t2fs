#ifndef BITMAP_H
#define BITMAP_H

#include "t2fs.h"

typedef enum{BLOCK_OCCUPIED, BLOCK_FREE} en_blockState;

int load_bitmap(super_block *sb);
int bitmapInit(super_block *sb);
void bitmap_flush(super_block *sb);
int set_block_free(WORD block);
int set_block_occupied(WORD block);
int get_free_block(void);

#endif
