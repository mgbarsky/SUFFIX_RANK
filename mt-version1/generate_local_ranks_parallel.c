#include <omp.h>
#include "utils.h"
#include "algorithm.h"

/**
   This sorts regions of SA with the same current_rank by their next_rank-
   the value a distance 2^h away.  It outputs runs of (curr, next, count)
 **/

//create RunRecord triplet and sort
void sort_and_output_group(int * sa_buffer, long * next_ranks_buffer, long current_rank,
                           int start_interval, int end_interval, FILE *runsFP){

	int i;
	tsort(&sa_buffer[start_interval], next_ranks_buffer, end_interval-start_interval);

	RunRecord output;
	output.currentRank = current_rank;
	output.count = 1;
	output.nextRank = next_ranks_buffer[sa_buffer[start_interval]];

  //find runs and write them to output
	for (i = start_interval+1; i < end_interval; i++) {
		if (next_ranks_buffer[sa_buffer[i]] != output.nextRank) {
			Fwrite (&output, sizeof(RunRecord), 1, runsFP);
			output.count = 1;
			output.nextRank = next_ranks_buffer[sa_buffer[i]];
		}
		else {
			output.count++;
		}
	}
	Fwrite (&output, sizeof(RunRecord), 1, runsFP);

}


int generate_local_runs_parallel (char * rank_dir, char * runs_dir, int total_chunks,
                         int chunk_id, int h) {

    //Determine which additional chunk must be loaded
   	int size_order = 0;
   	while((WORKING_CHUNK_SIZE >> size_order) > 1) {size_order++;}
   	int next_chunk_dist = h > size_order ? 1<<(h-size_order) : 0;
   	//printf("%d,%d\n",size_order,next_chunk_dist);
   	if ((next_chunk_dist + chunk_id) > total_chunks-1){
		// #pragma omp barrier
		// #pragma omp barrier
        return EMPTY;
    }


    char runs_file_name [MAX_PATH_LENGTH];
	FILE *runsFP = NULL;
 	sprintf (runs_file_name, "%s/runs_%d", runs_dir, chunk_id);
 	OpenBinaryFileAppend(&runsFP, runs_file_name);

	int i, r, total_records = 0;

	FILE *currentFP = NULL;
	FILE *nextFP = NULL;
	FILE *saFP = NULL;
	// FILE *summaryFP = NULL;

	// char summary_file_name [MAX_PATH_LENGTH];
	char current_ranks_file_name [MAX_PATH_LENGTH];
	char next_ranks_file_name [MAX_PATH_LENGTH];
	char sa_file_name [MAX_PATH_LENGTH];

	//allocate buffers
	long *current_ranks_buffer = (long *) Calloc ((WORKING_CHUNK_SIZE) *sizeof (long));
	long *next_ranks_buffer = (long *) Calloc ((WORKING_CHUNK_SIZE) *sizeof (long));
	int *sa_buffer = (int *) Calloc ((WORKING_CHUNK_SIZE) *sizeof (int));

	// sprintf (summary_file_name, "%s/merge_summary", runs_dir);
	sprintf (current_ranks_file_name, "%s/ranks_%d", rank_dir, chunk_id);
	sprintf (sa_file_name, "%s/sa_%d", rank_dir, chunk_id);

	//open current rank and sa file
	OpenBinaryFileRead (&currentFP, current_ranks_file_name);
	OpenBinaryFileReadWrite (&saFP, sa_file_name);
	// OpenBinaryFileWrite (&summaryFP, summary_file_name);

	//handle reading next_rank
	if (next_chunk_dist) {
		sprintf (next_ranks_file_name, "%s/ranks_%d", rank_dir, chunk_id+next_chunk_dist);
		OpenBinaryFileRead (&nextFP, next_ranks_file_name);
		fread (next_ranks_buffer, sizeof (long), WORKING_CHUNK_SIZE, nextFP);
	} else{
		sprintf (next_ranks_file_name, "%s/ranks_%d", rank_dir, chunk_id);
		OpenBinaryFileRead (&nextFP, next_ranks_file_name);
		if(fseek(nextFP, (1 << h)*sizeof(long), SEEK_SET)) {
			printf ("Fseek failed trying to move to position %d in ranks file\n", (1 << h));
			exit (1);
		}
		r = fread (next_ranks_buffer, sizeof (long), WORKING_CHUNK_SIZE, nextFP);
		fclose(nextFP);
		if (chunk_id+1 < total_chunks) {
			sprintf (next_ranks_file_name, "%s/ranks_%d", rank_dir, chunk_id+1);
			OpenBinaryFileRead (&nextFP, next_ranks_file_name);
			fread (next_ranks_buffer + r, sizeof (long), (1<<h), nextFP);
            fclose (nextFP);
		}
	}

	//offset next rank by 2^h

	//read file by chunk, sort and generate triplet for each chunk
	total_records=fread (current_ranks_buffer, sizeof (long), WORKING_CHUNK_SIZE, currentFP);
    fclose (currentFP);
	// fread (next_ranks_buffer, sizeof (long), WORKING_CHUNK_SIZE, nextFP);
	r = fread (sa_buffer, sizeof (int), WORKING_CHUNK_SIZE, saFP);
	if (r != total_records) {
		printf("Unexpected error: SA has different size %d than ranks array %d\n", r, total_records);
		return FAILURE;
	}
    // printf("Thread %d got here", omp_get_thread_num());
    // #pragma omp barrier
	int finished = 1;
	int start_interval = 0;
	int end_interval;
	long previous_rank = current_ranks_buffer[sa_buffer[0]];
	long current_rank;

  //Read through current_ranks_buffer until it changes.  Then sort based on next_rank.
	for (i=1; i < total_records; i++) {
		current_rank = current_ranks_buffer[sa_buffer[i]];
		if (current_rank != previous_rank) {
			if (previous_rank > 0) {
				finished = 0;
				end_interval = i;
				//sort, generate runs
				sort_and_output_group(sa_buffer, next_ranks_buffer, previous_rank,
				                      start_interval, end_interval, runsFP);
			}
			start_interval = i;
			previous_rank = current_rank;
		}
	}

	if (previous_rank > 0) {
		finished = 0;
		end_interval = total_records;
		sort_and_output_group(sa_buffer, next_ranks_buffer, previous_rank,
		                      start_interval, end_interval, runsFP);
	}

	fclose(runsFP);
	runsFP = NULL;

	// if (chunk_empty == 0) {
	//  finished = 0;
	//  Fwrite (&pos_infile, sizeof(int), 1, summaryFP);
	// }
	//return pointer to the beginning of the sa chunk
    // #pragma omp barrier
	fseek ( saFP, -(total_records )*sizeof(int), SEEK_CUR );
	Fwrite (sa_buffer, sizeof(int), total_records, saFP);
	// pos_infile += total_records;




	fclose(saFP);
	// fclose (summaryFP);


	free (sa_buffer);
	free (current_ranks_buffer);
	free (next_ranks_buffer);

	if (finished)
		return EMPTY;
	return SUCCESS;
}

int main(int argc, char ** argv){
	char * rank_dir;
	char * runs_dir;
	int h, chunk_id, total_chunks;
	if (argc<5) {
		puts ("Run ./generate_local_runs <rank_dir> <runs_dir> <total_chunks> <order>");
		return FAILURE;
	}

  //Read inputs
	rank_dir = argv[1];
	runs_dir = argv[2];
	total_chunks = atoi(argv[3]);
	h = atoi(argv[4]);

    int more_runs = EMPTY;
    #pragma omp parallel for schedule(static, 1) num_threads(NUM_THREADS) private(chunk_id)
    for (chunk_id = 0; chunk_id < total_chunks; chunk_id++){
        int result = generate_local_runs_parallel (rank_dir, runs_dir, total_chunks, chunk_id, h);
        if (result == FAILURE){
            more_runs = FAILURE;
        }
        if (result != EMPTY && more_runs != FAILURE){
            more_runs = SUCCESS;
        }
    }

	return more_runs;
}
