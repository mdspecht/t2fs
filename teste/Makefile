
CC=gcc
LIB_DIR=../lib
INC_DIR=../include
C_FLAGS=-m32 -Wall
C_PATH=./include

CFLAGS=-lt2fs -Wall -m32 -I$(INC_DIR)

all: format mount

format: format.c $(LIB_DIR)/libt2fs.a
	$(CC) -o format format.c -L$(LIB_DIR) $(CFLAGS)

mount: mount.c $(LIB_DIR)/libt2fs.a
	$(CC) -o mount mount.c -L$(LIB_DIR) $(CFLAGS)

.PHONY: all clean format mount

clean:
	rm -rf main *.o *~
