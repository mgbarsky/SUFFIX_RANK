CC = gcc
CFLAGS = -O3 -Wall
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -funroll-loops
WORKING_CHUNK_SIZE=33554432
CFLAGS += -DWORKING_CHUNK_SIZE=\$(WORKING_CHUNK_SIZE)

default: all

all: input_to_binary init glr ulr resolve_global_ranks glr_parallel ulr_parallel create_pairs create_pairs_parallel invert invert_parallel

mac: input_to_binary init glr ulr resolve_global_ranks glr_parallel_mac ulr_parallel_mac create_pairs create_pairs_parallel_mac invert invert_parallel_mac glr_parallel_internal

input_to_binary: input_to_binary.c utils.c
	$(CC) $(CFLAGS) input_to_binary.c utils.c -o input_to_binary

init: init_hash_table.c init.c utils.c
	$(CC) $(CFLAGS) init_hash_table.c init.c utils.c -o init

glr: generate_local_ranks.c utils.c
	$(CC) $(CFLAGS) generate_local_ranks.c utils.c -o generate_local_ranks

glr_parallel: generate_local_ranks_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) generate_local_ranks_parallel.c utils.c -o generate_local_ranks_parallel

glr_parallel_mac: generate_local_ranks_parallel.c utils.c
	gcc-9 -fopenmp $(CFLAGS) generate_local_ranks_parallel.c utils.c -o generate_local_ranks_parallel

glr_parallel_internal: generate_local_ranks_parallel_internal.c utils.c
	gcc-9 -fopenmp $(CFLAGS) generate_local_ranks_parallel_internal.c utils.c -o generate_local_ranks_parallel_internal

resolve_global_ranks: merge_runs.c utils.c
	$(CC) $(CFLAGS) merge_runs.c utils.c -o resolve_global_ranks

ulr: update_local_ranks.c utils.c
	$(CC) $(CFLAGS) update_local_ranks.c utils.c -o update_local_ranks

ulr_parallel: update_local_ranks_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) update_local_ranks_parallel.c utils.c -o update_local_ranks_parallel

ulr_parallel_mac: update_local_ranks_parallel.c utils.c
	gcc-9 -fopenmp $(CFLAGS) update_local_ranks_parallel.c utils.c -o update_local_ranks_parallel

# reducer: reduce_ranks.c
# 	$(CC) $(CFLAGS) reduce_ranks.c -o reduce

create_pairs: create_pairs.c utils.c
	$(CC) $(CFLAGS) create_pairs.c utils.c -o create_pairs

create_pairs_parallel: create_pairs_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) create_pairs_parallel.c utils.c -o create_pairs_parallel

create_pairs_parallel_mac: create_pairs_parallel.c utils.c
	gcc-9 -fopenmp $(CFLAGS) create_pairs_parallel.c utils.c -o create_pairs_parallel

invert: invert.c utils.c
	$(CC) $(CFLAGS) invert.c utils.c -o invert

invert_parallel: invert_parallel.c utils.c
	$(CC) -fopenmp $(CFLAGS) invert_parallel.c utils.c -o invert_parallel

invert_parallel_mac: invert_parallel.c utils.c
	gcc-9 -fopenmp $(CFLAGS) invert_parallel.c utils.c -o invert_parallel

# merge_sa_create_pairs_parallel_mac: merge_sa_create_pairs_parallel.c utils.c
# 	gcc-9 -fopenmp $(CFLAGS) merge_sa_create_pairs_parallel.c utils.c -o merge_sa_create_pairs_parallel
#
# merge_sa: merge_sa.c utils.c
# 	$(CC) $(CFLAGS) merge_sa.c utils.c -o merge_sa

clean:
	rm input_to_binary init generate_local_ranks generate_local_ranks_parallel generate_local_ranks_parallel_internal resolve_global_ranks update_local_ranks update_local_ranks_parallel create_pairs create_pairs_parallel invert invert_parallel
