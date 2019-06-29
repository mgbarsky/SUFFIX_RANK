#include "utils.h"
#include "algorithm.h"
#include <stdio.h>
#include <omp.h>

void invert(char * pairs_dir, char * output_dir, int chunk_id) {
	InverseRecord current_pair;
	char pairs_file_name[MAX_PATH_LENGTH];
	char output_file_name[MAX_PATH_LENGTH];

	FILE * pairsFP;
	FILE * outputFP;

	sprintf(pairs_file_name, "%s/pairs_%d", pairs_dir, chunk_id);
	sprintf(output_file_name, "%s/suffixarray_%d", output_dir, chunk_id);

	OpenBinaryFileRead(&pairsFP, pairs_file_name);

	long * suffixarray = (long *) Calloc(WORKING_CHUNK_SIZE * sizeof(long));
	long total_record = 0;
	// printf("still here %d\n", chunk_id);
	while (fread(&current_pair, sizeof(InverseRecord), 1, pairsFP)) {
		suffixarray[current_pair.value % WORKING_CHUNK_SIZE] = current_pair.index;
		total_record++;
	}

	OpenBinaryFileWrite(&outputFP, output_file_name);
	// printf("Chunk_id: %d, total_record: %ld\n", chunk_id, total_record);
	Fwrite(suffixarray, sizeof(long), total_record, outputFP);
	fclose(pairsFP);
	fclose(outputFP);
	free(suffixarray);
}

int main(int argc, char ** args) {
	char * pairs_dir;
	char * output_dir;
	int total_chunks, chunk_id;

	if (argc < 4) {
		puts ("Run ./invert <pairs_dir> <output_dir> <total_chunks>\n");
		printf("argc:%d\n", argc);
		return FAILURE;
	}

	pairs_dir = args[1];
	output_dir = args[2];
	total_chunks = atoi(args[3]);

	omp_set_num_threads(NUM_THREADS);
	for (chunk_id=0; chunk_id<total_chunks; chunk_id+=NUM_THREADS) {
		#pragma omp parallel
		{
			if (chunk_id+omp_get_thread_num() < total_chunks)
				invert(pairs_dir, output_dir, chunk_id+omp_get_thread_num());
		}
	}
	return SUCCESS;
}
