#include "utils.h"
#include "algorithm.h"
#include <stdio.h>
#include <stdlib.h>

int cmpfnc(const void * a, const void * b) {
	return ((InverseRecord *) a)->value - ((InverseRecord *) b)->value;
}

void create_pairs(char * ranks_dir, char * output_dir, int chunk_id) {
	int i, writing_file;

	InverseRecord output;

	char ranks_file_name[MAX_PATH_LENGTH];
	char output_file_name[MAX_PATH_LENGTH];

	long * current_rank = (long*)Calloc(WORKING_CHUNK_SIZE * sizeof(long));
	InverseRecord * inverse_buffer = (InverseRecord *) Calloc(WORKING_CHUNK_SIZE * sizeof(InverseRecord));

	FILE * ranksFP = NULL;
	FILE * outputFP = NULL;

	sprintf(ranks_file_name, "%s/ranks_%d", ranks_dir, chunk_id);
	OpenBinaryFileRead(&ranksFP, ranks_file_name);
	int num_elements = fread(current_rank, sizeof(long), WORKING_CHUNK_SIZE, ranksFP);
	fclose(ranksFP);

	for (i=0; i<num_elements; i++) {
		output.index = i + chunk_id * WORKING_CHUNK_SIZE;
		output.value = -1 * current_rank[i];
		inverse_buffer[i]	= output;
	}
	qsort(inverse_buffer, num_elements, sizeof(InverseRecord), cmpfnc);

	int last_file = -1;
	for (i=0; i<num_elements; i++) {
		writing_file = inverse_buffer[i].value / WORKING_CHUNK_SIZE;
		if (last_file != writing_file) {
			// printf("Writing to pairs_%d\n", writing_file);
			if (outputFP) {
				fclose(outputFP);
			}
			sprintf(output_file_name, "%s/pairs_%d", output_dir, writing_file);
			OpenBinaryFileAppend(&outputFP, output_file_name);
			last_file = writing_file;
		}
		fwrite(inverse_buffer+i, sizeof(InverseRecord), 1, outputFP);
	}
	// printf("Rank: %ld\n", current_rank[WORKING_CHUNK_SIZE-1]);
	if (outputFP) {
		fclose(outputFP);
	}
	free(current_rank);
	free(inverse_buffer);
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

	for (chunk_id = 0; chunk_id < total_chunks; chunk_id++) {
		create_pairs(ranks_dir, output_dir, chunk_id);
	}
	return SUCCESS;
}
