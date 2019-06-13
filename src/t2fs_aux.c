#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "t2fs.h"
#include "mbr.h"
#include "superBlock.h"
#include "t2fs_aux.h"
#include "dir.h"
#include "bitmap.h"

#define debug_printf printf

#define MAX_OPENED_FILES 10
#define MAX_OPENED_DIRS  30


struct fileDescriptor  OpenedFiles[MAX_OPENED_FILES];
struct dirDescriptor OpenedDirs[MAX_OPENED_DIRS];


char mounted=0;



int check_params(stMbr *mbr, WORD sectors_per_block)
{
	WORD blockSize= mbr->sectorSize* sectors_per_block;
	if(blockSize < sizeof(dirRecordDisk)){
		debug_printf("%s: error, blockSize(%d) not big enough to store a single fileEntry(%d)\n",__FUNCTION__,  blockSize, sizeof(dirRecordDisk));
		return -1;
	}
	return 0;
}



//Initialize all FS structures
void t2fs_mount(void)
{
	printf("%s: entered.\n", __FUNCTION__);
	if(!mounted){
		mounted=1;

		printf("teste1\n");
		load_MBR();
		printf("teste2\n");

		SB.load(MBR.entry[0].startSector);
		printf("teste3\n");

		print_SB();
		printf("teste4\n");
		
		memset(OpenedFiles, 0x00, sizeof(OpenedFiles));
		memset(OpenedDirs, 0x00, sizeof(OpenedFiles));
		memset(&RootDescriptor, 0x00, sizeof(RootDescriptor));
//		EmptyBlock= calloc(1, SB.getBlockSize());

		Bitmap.load(&SB);

		Bitmap.print();
		printf("teste5\n");

		root_load();
	}
}
