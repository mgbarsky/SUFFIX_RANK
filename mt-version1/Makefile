ifeq ($(shell uname -s), Darwin)
	CC = gcc-10
else
	CC = gcc
endif
CFLAGS = -O3 -Wall
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -funroll-loops

WORKING_CHUNK_SIZE=33554432
NUM_THREAD = 1
CFLAGS += -DNUM_THREAD=\$(NUM_THREAD)
CFLAGS += -DWORKING_CHUNK_SIZE=\$(WORKING_CHUNK_SIZE)

default: all

all: input_to_binary init  resolve_global_ranks generate_local_ranks_parallel update_local_ranks_parallel create_pairs_parallel invert_parallel

input_to_binary: input_to_binary.c utils.c
	$(CC) $(CFLAGS) input_to_binary.c utils.c -o input_to_binary

init: init_hash_table.c init.c utils.c
	$(CC) $(CFLAGS) init_hash_table.c init.c utils.c -o init

generate_local_ranks_parallel: generate_local_ranks_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) generate_local_ranks_parallel.c utils.c -o generate_local_ranks_parallel

resolve_global_ranks: merge_runs.c utils.c
	$(CC) $(CFLAGS) merge_runs.c utils.c -o resolve_global_ranks

update_local_ranks_parallel: update_local_ranks_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) update_local_ranks_parallel.c utils.c -o update_local_ranks_parallel

create_pairs_parallel: create_pairs_parallel.c utils.c
	$(CC) $(CFLAGS) create_pairs_parallel.c utils.c -o create_pairs_parallel

invert_parallel: invert_parallel.c utils.c
	$(CC) $(CFLAGS) invert_parallel.c utils.c -o invert_parallel

clean:
	rm input_to_binary init generate_local_ranks_parallel resolve_global_ranks update_local_ranks_parallel create_pairs_parallel invert_parallel
