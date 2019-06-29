#include "utils.h"
#include "algorithm.h"
#include <stdio.h>
#include <omp.h>

void create_pairs(char * ranks_dir, char * output_dir, int chunk_id) {
	int i, writing_file;

	InverseRecord output;

	char ranks_file_name[MAX_PATH_LENGTH];
	char sa_file_name[MAX_PATH_LENGTH];
	char output_file_name[MAX_PATH_LENGTH];

	long * current_rank = (long*)Calloc(WORKING_CHUNK_SIZE * sizeof(long));
	int * sa_buffer = (int *) Calloc(WORKING_CHUNK_SIZE * sizeof(int));

	FILE * ranksFP = NULL;
	FILE * saFP = NULL;
	FILE * outputFP = NULL;

	sprintf(ranks_file_name, "%s/ranks_%d", ranks_dir, chunk_id);
	OpenBinaryFileRead(&ranksFP, ranks_file_name);
	fread(current_rank, sizeof(long), WORKING_CHUNK_SIZE, ranksFP);
	fclose(ranksFP);

	sprintf(sa_file_name, "%s/sa_%d", ranks_dir, chunk_id);
	OpenBinaryFileRead(&saFP, sa_file_name);
	int num_elements = fread(sa_buffer, sizeof(unsigned int), WORKING_CHUNK_SIZE, saFP);
	fclose(saFP);

	int last_file = -1;
	for (i=0; i<num_elements; i++) {

		writing_file = -1 * current_rank[sa_buffer[i]] / WORKING_CHUNK_SIZE;
		if (last_file != writing_file) {
			// printf("Writing to pairs_%d\n", writing_file);
			if (outputFP) {
				fclose(outputFP);
			}
			sprintf(output_file_name, "%s/pairs_%d", output_dir, writing_file);
			OpenBinaryFileAppend(&outputFP, output_file_name);
			last_file = writing_file;
		}
		output.index = sa_buffer[i] + chunk_id * WORKING_CHUNK_SIZE;
		output.value = -1 * current_rank[sa_buffer[i]];
		fwrite(&output, sizeof(InverseRecord), 1, outputFP);
	}
	// printf("Rank: %ld\n", current_rank[WORKING_CHUNK_SIZE-1]);
	if (outputFP) {
		fclose(outputFP);
	}
	free(current_rank);
	free(sa_buffer);
}

int main(int argc, char ** args) {
	char * ranks_dir;
	char * output_dir;
	int total_chunks, chunk_id;

	if (argc < 4) {
		puts ("Run ./create_pairs <rank_dir> <output_dir> <total_chunks>");
		return FAILURE;
	}

	ranks_dir = args[1];
	output_dir = args[2];
	total_chunks = atoi(args[3]);

	omp_set_num_threads(NUM_THREADS);
	for (chunk_id = 0; chunk_id < total_chunks; chunk_id+=NUM_THREADS) {
		#pragma omp parallel
		{
			if (chunk_id+omp_get_thread_num() < total_chunks)
				create_pairs(ranks_dir, output_dir, chunk_id+omp_get_thread_num());
		}
	}
	return SUCCESS;
}
