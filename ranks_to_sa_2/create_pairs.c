#include "utils.h"
#include "algorithm.h"
#include <stdio.h>
#include <stdlib.h>

void create_pairs(char * ranks_dir, char * output_dir, int chunk_id, int total_chunks) {
	int i;
	InverseRecord output;

	char ranks_file_name[MAX_PATH_LENGTH];
	char output_file_name[MAX_PATH_LENGTH];

	long * current_rank = (long *) Calloc(WORKING_CHUNK_SIZE * sizeof(long));
	InverseRecord * inverse_buffer = (InverseRecord *) Calloc(WORKING_CHUNK_SIZE * sizeof(InverseRecord));

	FILE * ranksFP = NULL;
	FILE * outputFP = NULL;

	sprintf(ranks_file_name, "%s/ranks_%d", ranks_dir, chunk_id);

	OpenBinaryFileRead(&ranksFP, ranks_file_name);
	int num_elements = fread(current_rank, sizeof(long), WORKING_CHUNK_SIZE, ranksFP);
	fclose(ranksFP);

	int bucket_starts[total_chunks];
	for (i=0; i < total_chunks; i++) {
		bucket_starts[i] = 0;
	}

	for (i=0; i < num_elements; i++) {
		bucket_starts[-1 * current_rank[i] / WORKING_CHUNK_SIZE]++;
	}

	int sum = 0;
	int temp;
	for (i=0; i < total_chunks; i++) {
		temp = bucket_starts[i];
		bucket_starts[i] = sum;
		sum += temp;
	}

	for (i=0; i < num_elements; i++) {
		output.index = i + chunk_id * WORKING_CHUNK_SIZE;
		output.value = -1 * current_rank[i];
		inverse_buffer[bucket_starts[output.value / WORKING_CHUNK_SIZE]++] = output;
	}

	sprintf(output_file_name, "%s/pairs_0", output_dir);
	OpenBinaryFileAppend(&outputFP, output_file_name);
	fwrite(inverse_buffer, sizeof(InverseRecord), bucket_starts[0], outputFP);
	fclose(outputFP);

	for (i=1; i < total_chunks; i++) {
		sprintf(output_file_name, "%s/pairs_%d", output_dir, i);
		OpenBinaryFileAppend(&outputFP, output_file_name);
		fwrite(inverse_buffer+bucket_starts[i-1], sizeof(InverseRecord), bucket_starts[i]-bucket_starts[i-1], outputFP);
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

	printf("total chunks: %d\n", total_chunks);
	for (chunk_id = 0; chunk_id < total_chunks; chunk_id++) {
		create_pairs(ranks_dir, output_dir, chunk_id, total_chunks);
	}
	return SUCCESS;
}
