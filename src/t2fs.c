
/**
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "t2fs.h"
#include "bitmap.h"
#include "aux.h"
#include "debug.h"

super_block SuperBlock;

BYTE Mounted=0;
BYTE workBuffer[SECTOR_SIZE];
extern BYTE *Bitmap;


char *currPath=NULL;


/*-----------------------------------------------------------------------------
Função:	Realiza a montagem do sistema de arquivos T2FS.
-----------------------------------------------------------------------------*/
int mount(void){
	BYTE *mbrBuffer = malloc(SECTOR_SIZE);
	if (mbrBuffer==NULL){
		debug_printf("error at alloc in %s.\n", __FUNCTION__);
		return -1;
	}

	if(read_MBR(mbrBuffer)!=0){
		return -1;
	}

	debug_printf("hello\n\n\n");
	if(Mounted==0){
		Mounted= 1;
		load_superBlock( MBR_START_SECTOR(mbrBuffer), &SuperBlock);
		print_SB(&SuperBlock);
		load_bitmap(&SuperBlock);

		currPath= malloc(SECTOR_SIZE*SuperBlock.blockSize);	
		strcpy(currPath,"/");
	}
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Informa a identificação dos desenvolvedores do T2FS.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
    const char* names = "Jessica Maria Lorencetti - 228342\nMarcelo Dutra Specht - 230090\nRicardo de Araujo Coelho - 160542\n";
    if(strlen(names)<=size){
        strncpy(name,names,size);
        return 0;
    }
    else{
        return -1;
    }
}
/*-----------------------------------------------------------------------------
Função:	Formata logicamente o disco virtual t2fs_disk.dat para o sistema de
		arquivos T2FS definido usando blocos de dados de tamanho 
		corresponde a um múltiplo de setores dados por sectors_per_block.
-----------------------------------------------------------------------------*/
int format2 (int sectors_per_block)
{
	BYTE *mbrBuffer = malloc(SECTOR_SIZE);
	Mbr *mbr= (Mbr*)mbrBuffer;
	super_block *sb= &SuperBlock;
	if(mbrBuffer==NULL){
		return -1;
	}
	if(read_MBR(mbrBuffer)!=0){
		return -1;
	}
	sb->id[0]='J';
	sb->id[1]='M';
	sb->id[2]='R';
	sb->id[3]='\0';
	sb->blockSize= sectors_per_block;
	sb->startSector= mbr->entry[0].startAddr;

	sb->numBlocks= 1+ (mbr->entry[0].endAddr - mbr->entry[0].startAddr)/sb->blockSize;
	debug_printf("numBlocks:: %d\n", sb->numBlocks);

	bitmapInit(sb);

	sb->root_IB= get_free_block();
	if(sb->root_IB !=0){
		printf("error at getting BI Block for root dir at %s\n.",__FUNCTION__);
	}
	

	BYTE* blk_buffer= NULL;

	sb->root_IB= IB_alloc(sb, blk_buffer);
	if(sb->root_IB!=0){
		return -1;
	}

	fileEntry entry;
	strcpy(entry.name, ".");
	entry.fileType= FILE_DIR;
	entry.index_block= sb->root_IB;

	IB_add_Entry(sb, blk_buffer, &entry);

	memset(entry.name, 0x00, MAX_FILE_NAME_SIZE);
	strcpy(entry.name, "..");

	IB_add_Entry(sb, blk_buffer, &entry);

	blk_mem_to_disk(sb, blk_buffer, sb->root_IB);

	superBlock_to_disk(sb);

	currPath= malloc(SECTOR_SIZE*sb->blockSize);	
	strcpy(currPath,"/");

	debug_printf("exiting %s.\n",__FUNCTION__);
	free(mbrBuffer);
	return 0;
}


/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo arquivo no disco e abrí-lo,
		sendo, nesse último aspecto, equivalente a função open2.
		No entanto, diferentemente da open2, se filename referenciar um 
		arquivo já existente, o mesmo terá seu conteúdo removido e 
		assumirá um tamanho de zero bytes.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
	
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco. 
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo. Remove do arquivo 
		todos os bytes a partir da posição atual do contador de posição
		(current pointer), inclusive, até o seu final.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Altera o contador de posição (current pointer) do arquivo.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo diretório.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname) {

	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um diretório do disco.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para alterar o CP (current path)
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para obter o caminho do diretório corrente.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink) com
		o nome dado por linkname (relativo ou absoluto) para um 
		arquivo ou diretório fornecido por filename.
-----------------------------------------------------------------------------*/
int ln2 (char *linkname, char *filename) {
	return -1;
}


