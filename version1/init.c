#include "utils.h"
#include "init_hash_table.h"
#include "time.h"

clock_t start, end;
double time_read;
double time_total, time_write = 0.0;
clock_t start_while, end_while;


//cmpfunction for quicksorting array of characters
int cmpfunc( const void *a, const void *b) {
	return *(unsigned int *)a - *(unsigned int *)b;
}

//generating the suffix array for iteration 0 with bucket sorting
void init_sa (unsigned int * input_buffer, int * sa_buffer, HNode ** hashtable,
              unsigned int * inter_arr, int char_count, int total){
	HNode *np;
	int i;
	int sum = 0, temp = 0;
	//reevaluating count into start position of each character
	for (i = 0; i < char_count; i++) {
		np = lookup(hashtable, DEFAULT_HASH_SIZE, inter_arr[i]);
		temp = np->count;
		np->count = sum;
		sum += temp;
	}
	//put each character in the string into the first available spot in its bucket
	for (i = 0; i < total; i++) {
		np = lookup(hashtable, DEFAULT_HASH_SIZE, input_buffer[i]);
		sa_buffer[np->count] = i;
		np->count++;
	}
	//reset count
	for (i = 0; i < char_count; i++) {
		np = lookup(hashtable, DEFAULT_HASH_SIZE, inter_arr[i]);
		np->count = 0;
	}
}


int count_characters (char *input_directory, char * output_directory) {
	char input_file_name [MAX_PATH_LENGTH];
	char output_file_name [MAX_PATH_LENGTH];
	char sa_file_name [MAX_PATH_LENGTH];

	HNode *np;
	int i, read;
	unsigned int * inter_arr;
	unsigned int * buffer;
	int char_count = 0;

	FILE * inputFP=NULL;
	FILE * outputFP = NULL;
	FILE * saFP = NULL;

	sprintf (input_file_name, "%s/binary_input", input_directory);
	//buffer to read character in file
	buffer = (unsigned int *) Calloc (WORKING_CHUNK_SIZE*sizeof(unsigned int));
	//create hastable for storing character in file
	static HNode* hashtable[DEFAULT_HASH_SIZE];

	OpenBinaryFileRead (&inputFP, input_file_name);

	if (TEST_PERFORMANCE) {
		start = clock();
	}

	while ((read = fread (buffer,sizeof(unsigned int),WORKING_CHUNK_SIZE,inputFP)) > 0) {
		for (i=0; i < read; i++) {
			if (insert(hashtable, DEFAULT_HASH_SIZE, buffer[i])) char_count++;
		}
	}

	if (TEST_PERFORMANCE) {
		end = clock();
		time_read = (double)(end-start) / CLOCKS_PER_SEC;
		printf("Global character count in %.4f\n",time_read);
	}

	//now merge counts - later with map-reduce
	inter_arr = Calloc (char_count*sizeof(unsigned int));

	int index = 0;
	for (i=0; i<DEFAULT_HASH_SIZE; i++) { //iterate over hash array
		if (hashtable [i]!=NULL) { //if there is an entry at position i
			HNode * curr = hashtable [i];
			while (curr != NULL) {
				inter_arr[index] = curr->key;
				index++;
				curr = curr->next;
			}
		}
	}

	//quicksort all unique character lexicographically
	qsort(inter_arr, char_count, sizeof(unsigned int), cmpfunc);
	long rank = 0;

	//calculating rank for each unique character
	for (i = 0; i < char_count; i++) {
		np = lookup(hashtable, DEFAULT_HASH_SIZE, inter_arr[i]);
		np->rank = rank;
		rank += np->count;
		np->count = 0;
	}

	long * output_buffer = (long *) Calloc (WORKING_CHUNK_SIZE*sizeof(long));
	int * sa_buffer = (int *) Calloc (WORKING_CHUNK_SIZE*sizeof(int));

	int pos_in_buffer = 0;
	rewind(inputFP);

	int chunk_id = 0;
	int current_sentinel = 0;

	if (TEST_PERFORMANCE) {
		start_while = clock();
	}
	//write rank array and suffix array to file by chunk
	while ((read = fread (buffer,sizeof(unsigned int),WORKING_CHUNK_SIZE,inputFP)) > 0) {
		for (i=0; i < read; i++) {
			np =lookup(hashtable, DEFAULT_HASH_SIZE, buffer[i]);
			if (buffer[i]==0) {
				output_buffer[pos_in_buffer++] = current_sentinel;
				current_sentinel--;
			}
			else {
				output_buffer[pos_in_buffer++] = np->rank;
			}
			np->count++;

			if (pos_in_buffer == WORKING_CHUNK_SIZE) {
				sprintf(output_file_name, "%s/ranks_%d", output_directory, chunk_id);
				sprintf(sa_file_name, "%s/sa_%d", output_directory, chunk_id);
				OpenBinaryFileWrite (&outputFP, output_file_name);
				OpenBinaryFileWrite (&saFP, sa_file_name);

				//bucket sort, make suffix array
				init_sa(buffer, sa_buffer, hashtable, inter_arr, char_count, pos_in_buffer);

				if (TEST_PERFORMANCE)
					start = clock();

				Fwrite (output_buffer, sizeof(long), pos_in_buffer, outputFP);
				Fwrite (sa_buffer, sizeof(int), pos_in_buffer, saFP);

				if (TEST_PERFORMANCE) {
					end = clock();
					time_write += (double)(end-start) / CLOCKS_PER_SEC;
				}

				fclose(outputFP);
				fclose(saFP);

				chunk_id++;
				pos_in_buffer = 0;


			}
		}
	}

	if (pos_in_buffer > 0) {
		if (TEST_PERFORMANCE)
			start = clock();

		sprintf(output_file_name, "%s/ranks_%d", output_directory, chunk_id);
		sprintf(sa_file_name, "%s/sa_%d", output_directory, chunk_id);
		OpenBinaryFileWrite (&outputFP, output_file_name);
		OpenBinaryFileWrite (&saFP, sa_file_name);

		Fwrite (output_buffer, sizeof(long), pos_in_buffer, outputFP);
		init_sa(buffer, sa_buffer, hashtable, inter_arr, char_count, pos_in_buffer);
		Fwrite (sa_buffer, sizeof(int), pos_in_buffer, saFP);

		fclose(outputFP);
		fclose(saFP);
		if (TEST_PERFORMANCE) {
			end = clock();
			time_write += (double)(end-start) / CLOCKS_PER_SEC;
		}
	}

	if (TEST_PERFORMANCE) {
		end_while = clock();
		time_total = (double)(end_while-start_while) / CLOCKS_PER_SEC;
		printf("Init sa and rank in %.4f, write:%.4f\n",time_total, time_write);
	}

	free(inter_arr);
	free(buffer);
	free(output_buffer);
	free(sa_buffer);
	fclose(inputFP);
	free_hashtable(hashtable, DEFAULT_HASH_SIZE);

	return SUCCESS;
}

/*
 **main for counting number of characters in binary file and write result into a file as 255-size array of long counts
 */
int main (int argc, char **argv){
	char * input_directory;
	char * temp_dir;
	printf("Current chunk size is set to %ld bytes\n", (long) WORKING_CHUNK_SIZE);
	if (argc<3) {
		puts ("Run ./count_characters <binary_inputs_dir> <temp_dir>");
		return FAILURE;
	}

	input_directory = argv[1];
	temp_dir = argv[2];

	return count_characters (input_directory, temp_dir);
}
