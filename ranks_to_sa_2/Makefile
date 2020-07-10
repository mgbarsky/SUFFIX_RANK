CC = gcc
CFLAGS = -O3 -Wall
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -funroll-loops
WORKING_CHUNK_SIZE=33554432
CFLAGS += -DWORKING_CHUNK_SIZE=\$(WORKING_CHUNK_SIZE)

default: all

all: create_pairs invert

mac: create_pairs invert

create_pairs: create_pairs.c utils.c
	$(CC) $(CFLAGS) create_pairs.c utils.c -o create_pairs

invert: invert.c utils.c
	$(CC) $(CFLAGS) invert.c utils.c -o invert

clean:
	rm create_pairs invert
