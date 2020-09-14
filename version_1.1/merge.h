#ifndef REDUCE_H
#define REDUCE_H

#include "utils.h"
#include "algorithm.h"

#define MAX_MEM_INPUT_BUFFERS (14 * WORKING_CHUNK_SIZE)
#define MAX_MEM_OUTPUT_BUFFERS (14 * WORKING_CHUNK_SIZE)

typedef struct heap_element {
	long current_rank;
	long next_rank;
	int count;
	int chunk_id;
} HeapElement;

typedef struct output_element {
	long new_rank;
	int chunk_id;
} OutputElement;

typedef struct merge_manager {
	long pair_count;
	long updated_rank; //the new rank obtained by adding count to the prev value of the updated_rank
	HeapElement *heap;  //keeps 1 from each buffer in top-down order - smallest on top (according to compare function)
	HeapElement last_transferred;             //last element transferred from heap to output buffer

	int *input_file_positions;             //current position in each file, -1 if the run is complete

	RunRecord **input_buffers; //array of buffers to hold part of each run
	int *input_buffer_positions; //position in current input buffer, if no need to refill  - -1
	int input_buffer_capacity; //how many elements max can each hold
	int *input_buffer_lengths;  //number of actual elements currently in input buffer - can be less than max capacity

	long** output_buffers;             //buffers to store output elements - in this case updated ranks - for each chunk until they are flushed to disk
	int *output_buffer_positions;              //where to add next element in each output buffer
	int output_buffer_capacity;             //how many elements max each output buffer can hold

	int current_heap_size;
	int total_chunks;
	char input_dir [MAX_PATH_LENGTH];              //to generate input file based on all file_id - interval_id combination listed in inputFileNumbers
	char output_dir [MAX_PATH_LENGTH];             //where to write merged updates for each fileid-intervalid
}Manager;

int reduce(char* input_dir, char* temp_dir, int total_chunks);
void setup(Manager * manager);
void clean_up(Manager * manager);
void flush_output_buffers (Manager *manager, int chunk_id);
int refill_buffer (Manager * manager, int chunk_id);
void heap_to_output_last ( Manager *manager, HeapElement *current, OutputElement *result);
void heap_to_output ( Manager *manager, HeapElement *current, OutputElement *result);
int get_next_input_element (Manager * manager, int chunk_id, RunRecord *result);
int insert_into_heap (Manager * manager, int chunk_id, RunRecord *input);
int get_top_heap_element (Manager * manager, HeapElement * result);
int init_merge (Manager * manager);
int merge_runs (Manager * manager);
long compare_heap_elements (HeapElement *a, HeapElement *b);

#endif
