#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "t2fs.h"
#include "t2fs_aux.h"
#include "superBlock.h"
#include "bitmap.h"
#include  "dir.h"

#include "debug.h"

#define debug_printf printf

//BYTE *EmptyBlock;
dirDescriptor RootDescriptor;
char * CurrPath;


#if 0
struct dirDescriptor
{
	char name[MAX_FILENAME_MAX_SIZE];
   	DWORD numRecords;
	DWORD currRecordOffset;
	struct dirRecordNode *firstRecord;
	struct dirRecordNode *currRecord;	
};
#endif



//#define NOT_EMPTY(blk) memcmp(blk, EmptyBlock)
#define IS_LAST_DATA_BLOCK_ENTRY(incPtr, basePtr) (((BYTE*)incPtr - basePtr)== SB.getBlockSize() - sizeof(WORD))

#define DIR_RECORD_DISK_ENTRY_NOT_EMPTY(RCRD, emptyRCRD) (memcmp(RCRD,emptyRCRD, sizeof(dirRecordDisk)) )

static int count_entries(BYTE *dataBlock)
{
	dirRecordDisk *record= (dirRecordDisk *)dataBlock;
	dirRecordDisk emptyRecord= {0};
	WORD numEntriesPerBlock= SB.getBlockSize()/sizeof(dirRecordDisk);
	int i;
	WORD numEntries=0;
	for(i=0;i<numEntriesPerBlock;i++){
		if(DIR_RECORD_DISK_ENTRY_NOT_EMPTY(&record[i], &emptyRecord)){
			numEntries++;
		}
	}
	return numEntries;
}

WORD dir_count_entries(BYTE *indexBlockBuffer)
{
	WORD* numDataBlock=(WORD*)indexBlockBuffer;
	BYTE *dataBuffer= malloc(SB.getBlockSize());
	WORD totalCount= 0;
	while(*numDataBlock != 0x0000){
		if(IS_LAST_DATA_BLOCK_ENTRY(numDataBlock, indexBlockBuffer)){
			BYTE *nextIndexBlockBuffer= malloc(SB.getBlockSize());
			block_disk_to_mem(*numDataBlock, nextIndexBlockBuffer);
			totalCount += dir_count_entries(nextIndexBlockBuffer);
			free(dataBuffer);
			free(nextIndexBlockBuffer);
			return totalCount;
		}
		block_disk_to_mem(*numDataBlock, dataBuffer);
		totalCount += count_entries(dataBuffer);
		numDataBlock++;
	}

	free(dataBuffer);
	return totalCount;

}

struct IterationCtrl
{
	BYTE *IB_buffer;
	BYTE *data_buffer;
	WORD block;
	WORD currEntry;

};

#define IB_ITERATOR_NEXT	0
#define IB_ITERATOR_INIT	1

#define LAST_IB_POSITION 	(SB.getBlockSize()/sizeof(dirRecordDisk)-1)
//iterate over index_block, returning a pointer to data_block
BYTE* IB_iterator(struct IterationCtrl *it, char state)
{
	switch(state){
		case IB_ITERATOR_INIT:
			it->IB_buffer= malloc(SB.getBlockSize());
			it->data_buffer= malloc(SB.getBlockSize());
			block_disk_to_mem(it->block, it->IB_buffer);
			it->currEntry=0;
			break;

		case IB_ITERATOR_NEXT:
			{
				WORD dataBlock= ((WORD *)it->IB_buffer)[it->currEntry];

				if(it->currEntry == LAST_IB_POSITION && dataBlock!=0){
					block_disk_to_mem(dataBlock, it->IB_buffer);
					it->currEntry=0;
					dataBlock= ((WORD *)it->IB_buffer)[it->currEntry];
				}
				if(dataBlock==0){
					//finished all entries
					free(it->IB_buffer);
					free(it->data_buffer);
					return NULL;
				}
				block_disk_to_mem(dataBlock, it->data_buffer);
				it->currEntry++;
				return it->data_buffer;
			}
	}
	return NULL;
}


#define MAX_RECORDS_PER_DATA_BLOCK		(SB.getBlockSize()/sizeof(dirRecordDisk))


/* dir_copy_disk_records:
 * Dado um index_node de um diretorio, varre todos os blocos de dados e coleta
 *  os registros de arquivos. Usado para se construir o dirDescriptor.
 *
 * parametros:
 * 	block: bloco de indice do diretorio a ser lido
 * 	node : estrutura de lista encadeada de nodo a ser preenchido por
 * 	 todas as entradas do diretorio. Tipicamente NULL
 * retorno:
 * 	Endereço do primeiro nodo da lista
 */
struct dirRecordNode * dir_copy_disk_records(WORD block, struct dirRecordNode *node)
{
	struct IterationCtrl it;
	it.block=block;
	BYTE* data_buffer;
	struct dirRecordNode *ret_node= node;
	struct dirRecordNode *it_node= node;
	while(it_node!=NULL && it_node->next!=NULL){
		it_node=it_node->next;
	}

	debug_printf("%s, 1: \n",__FUNCTION__);
	IB_iterator(&it, IB_ITERATOR_INIT);

	while((data_buffer= IB_iterator(&it,IB_ITERATOR_NEXT))!=NULL){
		dirRecordDisk *rec;
		rec= (dirRecordDisk*)data_buffer;
		int i;
		debug_printf("%s, 2: MAX_RECS_PER_DATA_BLOCK: %d\n",__FUNCTION__, MAX_RECORDS_PER_DATA_BLOCK);
		for(i=0; i < MAX_RECORDS_PER_DATA_BLOCK; i++){
			debug_printf("%s, 3: rec[%d].index_block: %d\n",__FUNCTION__, i, rec[i].index_block);

			if(rec[i].index_block!=0){
				struct dirRecordNode* newNode= malloc(sizeof(struct dirRecordNode));
				memcpy(&(newNode->entry), &rec[i], sizeof(dirRecordDisk));

				newNode->next=NULL;
				if(it_node==NULL){
					it_node= newNode;
					ret_node= it_node;
				}else{
					it_node->next= newNode;
					it_node= newNode;
				}
			}
		}
	}
	debug_printf("%s: 4\n", __FUNCTION__);
	return ret_node;
}

int addFile(dirDescriptor *dir, char *name, BYTE type, WORD index_block)
{
	dirRecordNode *node= malloc(sizeof(dirRecordNode));
	if(node==NULL){
		return -1;
	}
	memset(node->entry.attr.name, 0, sizeof(node->entry.attr.name));
	strcpy(node->entry.attr.name, name);
	node->entry.attr.fileType= type;
	node->entry.attr.fileSize=0;
	node->entry.index_block= index_block;
	node->next=NULL;

	if(dir->firstRecord == NULL){
		dir->firstRecord= node;
	}else{
		dirRecordNode *record= dir->firstRecord;
		while(record && record->next){ record= record->next;}
		if(record==NULL){
			dir->firstRecord= node;
		}else{
			record->next= node;
		}
	}

	dir->numRecords++;
	return 0;
}


void printEntry(dirRecordDisk entry)
{
	debug_printf("%s: name: %s.\n", __FUNCTION__, entry.attr.name);
	debug_printf("%s: type: %d.\n", __FUNCTION__, entry.attr.fileType);
	debug_printf("%s: size: %d.\n", __FUNCTION__, entry.attr.fileSize);
	debug_printf("%s: IB  : %d.\n", __FUNCTION__, entry.index_block);
}


/* algoritmo:
 * empacotar todas as entradas em buffers de dados,
 * salvar no disco esses blocos e anotar o numero no bloco de indices.
 * sei la!!! 
 * 
 */

void dir_store(dirDescriptor *dir)
{
//	int numDataBlocks= 1+ (dir->numRecords -1 )/MAX_RECORDS_PER_DATA_BLOCK;
	dirRecordNode *node= dir->firstRecord;
	WORD currDirIB= dir->indexBlock;
	union{
		BYTE *buffer;
		dirRecordDisk *entry;
	} dataBuffer;
	WORD entry_number=0;
	dataBuffer.buffer= calloc(1,SB.getBlockSize());
	BYTE* indexBlockBuffer= calloc(1,SB.getBlockSize());
	int i=0;
	if(!node)
		debug_printf("%s: NODE==NULL!\n", __FUNCTION__);
	while(node!=NULL){
		if(i==MAX_RECORDS_PER_DATA_BLOCK){
			printf("%s: max_records_per_data_block(i=%d).\n", __FUNCTION__, i);
			printf("%s: size: %d\n",__FUNCTION__, SB.getBlockSize());
			printf("%s: dirRecordDiskSize: %d\n",__FUNCTION__, sizeof(dirRecordDisk));
			//save this datablock in disk...
			WORD data_index= Bitmap.getFreeBlock();
			block_mem_to_disk(data_index, dataBuffer.buffer);
			//save datablock_index in indexBlock...
			((WORD*)indexBlockBuffer)[entry_number++]= data_index;
			if(entry_number==LAST_IB_POSITION){
				//indexBlock fully occupied. Store and alloc another...
				WORD newIndexBlock= Bitmap.getFreeBlock();
				((WORD*)indexBlockBuffer)[entry_number]= newIndexBlock;
				block_mem_to_disk(currDirIB, indexBlockBuffer);

				currDirIB= newIndexBlock;
				entry_number=0;
				memset(indexBlockBuffer, 0, SB.getBlockSize());
			}
			//and create another datablock.
			memset(dataBuffer.buffer, 0, SB.getBlockSize());
			i=0;

		}else{
			printf("ENTERED_HERE!\n");
			dataBuffer.entry[i++]= node->entry;
			printEntry(dataBuffer.entry[i-1]);

			node= node->next;
		}

	}
	//save last dataBlock in currIndexBlock 
	if(i>0){
		WORD data_index= Bitmap.getFreeBlock();
			block_mem_to_disk(data_index, dataBuffer.buffer);
		//save datablock_index in indexBlock...

		((WORD*)indexBlockBuffer)[entry_number]= data_index;
		block_mem_to_disk(currDirIB, indexBlockBuffer);
	}

	free(dataBuffer.buffer);
	free(indexBlockBuffer);
}



static int addDefaultDirs(dirDescriptor *dir, WORD parent_iblock)
{
	int ret;
	ret= addFile(dir, ".", FILE_DIR, dir->indexBlock);
	debug_printf("%s: ret: %d\n",__FUNCTION__, ret);
	ret= addFile(dir, "..", FILE_DIR, parent_iblock);
	debug_printf("%s: ret: %d\n",__FUNCTION__, ret);

	return 0;
}

int root_init(st_superBlock *sb)
{
	int root_block= Bitmap.getFreeBlock();
	if(root_block <0){
		debug_printf("error at getting BI Block for root dir at %s\n.",__FUNCTION__);
		return -1;
	}
	//Bitmap.markBlock((WORD)root_block, BLOCK_OCCUPIED);

	sb->setRootIndexBlock((WORD)root_block);
	
	strcpy(RootDescriptor.name,"/");
	RootDescriptor.indexBlock= root_block;
	RootDescriptor.numRecords=0;
	RootDescriptor.currRecordOffset=0;
	RootDescriptor.firstRecord= NULL;
	RootDescriptor.currRecord= NULL;

	addDefaultDirs(&RootDescriptor, RootDescriptor.indexBlock);

	dir_store(&RootDescriptor);

	CurrPath= malloc(sb->getBlockSize());	
	strcpy(CurrPath,"/");

	return 0;
}

void print_dir(const dirDescriptor *dir)
{
	printf("%s:\n", __FUNCTION__);
	printf("\tdir->name       : %s\n", dir->name);
	printf("\tdir->indexBlock : %d\n", dir->indexBlock);
	printf("\tdir->numRecords : %d\n", dir->numRecords);
	dirRecordNode * tmpRec= dir->firstRecord;
	int i=0;
	while(tmpRec){
		printf("\tentry[%d]: \n",i);
		printf("\t\tindexBlock			   : %d\n", tmpRec->entry.index_block);
		printf("\t\tname                   : %s\n", tmpRec->entry.attr.name);
		printf("\t\ttype[1:regular, 2:dir] : %d\n", tmpRec->entry.attr.fileType);
		printf("\t\tfileSize               : %d\n", tmpRec->entry.attr.fileSize);
		tmpRec= tmpRec->next;
		i++;
	}
}	

void root_load(void)
{
	BYTE *rootIndexBlockBuffer= malloc(SB.getBlockSize());
	
	printf("%s: 1\n", __FUNCTION__);
	strncpy(RootDescriptor.name, "/", MAX_FILE_NAME_SIZE);

	block_disk_to_mem(SB.getRootIndexBlock(), rootIndexBlockBuffer);
	RootDescriptor.indexBlock= SB.getRootIndexBlock();
   	RootDescriptor.numRecords= dir_count_entries(rootIndexBlockBuffer);
	printf("%s: 2\n",  __FUNCTION__);
	printf("%s: Root num Records: %d\n",  __FUNCTION__, RootDescriptor.numRecords);
	
	printf("%s: calling dir_copy_disk(%d, %p)\n",  __FUNCTION__, SB.getRootIndexBlock(), RootDescriptor.firstRecord );
	RootDescriptor.firstRecord= dir_copy_disk_records(SB.getRootIndexBlock(), RootDescriptor.firstRecord);

	print_dir(&RootDescriptor);

	printf("%s: 3\n",  __FUNCTION__);
	RootDescriptor.currRecordOffset= 0;
	RootDescriptor.currRecord= RootDescriptor.firstRecord;

	free(rootIndexBlockBuffer);
}

