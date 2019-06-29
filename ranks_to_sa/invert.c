#include "utils.h"
#include "algorithm.h"
#include <stdio.h>

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
	fseek (pairsFP, 0, SEEK_END);
	long total_record = ftell (pairsFP)/sizeof(InverseRecord);
	rewind(pairsFP);
	// int i = 0;
	while (fread(&current_pair, sizeof(InverseRecord), 1, pairsFP)) {
		// if (i > 200) {
		//   printf("index=%ld, value=%ld, value_mod_size=%ld\n", current_pair.index, current_pair.value, current_pair.value % WORKING_CHUNK_SIZE);
		// }
		suffixarray[current_pair.value % WORKING_CHUNK_SIZE] = current_pair.index;
		// i++;
	}

	OpenBinaryFileWrite(&outputFP, output_file_name);
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
		puts ("Run ./invert <pairs)dir> <output_dir> <total_chunks>");
		return FAILURE;
	}

	pairs_dir = args[1];
	output_dir = args[2];
	total_chunks = atoi(args[3]);

	for (chunk_id=0; chunk_id<total_chunks; chunk_id++) {
		invert(pairs_dir, output_dir, chunk_id);
	}
	return SUCCESS;
}
