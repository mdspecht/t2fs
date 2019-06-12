#include "mbr.h"
#include "apidisk.h"
#include <stdio.h>

#include "debug.h"

stMbr MBR;

#define debug_printf printf

void print_MBR(stMbr *mbr)
{
	debug_printf("MBR:\n");
	debug_printf("version:\t %04X\n",mbr->version);
	debug_printf("sectorSize:\t %d\n",mbr->sectorSize);
	debug_printf("partitionTableOffet: 0x%04X\n",mbr->partitionTableOffet);
	debug_printf("numPartitions: %d\n",mbr->numPartitions);

	debug_printf("entry[0].startSector:\t 0x%X\n",mbr->entry[0].startSector);
	debug_printf("entry[0].endSector:\t 0x%X\n",mbr->entry[0].endSector);
	debug_printf("entry[0].name:\t %24s\n",mbr->entry[0].name);
	
	debug_printf("entry[1].startSector:\t 0x%X\n",mbr->entry[1].startSector);
	debug_printf("entry[1].endSector:\t 0x%X\n",mbr->entry[1].endSector);
	debug_printf("entry[1].name:\t %24s\n",mbr->entry[1].name);
	
	debug_printf("entry[2].startSector:\t 0x%X\n",mbr->entry[2].startSector);
	debug_printf("entry[2].endSector:\t 0x%X\n",mbr->entry[2].endSector);
	debug_printf("entry[2].name:\t %24s\n",mbr->entry[2].name);
	
	debug_printf("entry[3].startSector:\t 0x%X\n",mbr->entry[3].startSector);
	debug_printf("entry[3].endSector:\t 0x%X\n",mbr->entry[3].endSector);
	debug_printf("entry[3].name:\t %24s\n",mbr->entry[3].name);
}

int load_MBR(void)
{
	if(read_sector(0, (BYTE*)&MBR)!=0){
		debug_printf("error at %s\n",__FUNCTION__);
		return -1;
	}
	print_MBR(&MBR);
	return 0;
}
