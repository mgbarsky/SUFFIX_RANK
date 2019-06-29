#include "utils.h"
#include "algorithm.h"
#include <limits.h>

int update_local_ranks (char * rank_dir, char * temp_dir, int total_chunks, int chunk_id, int h,
						long * buffer_current, long * buffer_next, int * sa_buffer, long * updated_ranks){
	int size_order = 0;
	while((WORKING_CHUNK_SIZE >> size_order) > 1) {size_order++;}
	int next_chunk_dist = h > size_order ? 1<<(h-size_order) : 0;
	//printf("%d,%d\n",size_order,next_chunk_dist);
	if ((next_chunk_dist + chunk_id) > total_chunks-1) return EMPTY;

	char file_name[MAX_PATH_LENGTH];

	FILE * global_resolved_FP = NULL;
	FILE * current_FP = NULL;
	FILE * next_FP = NULL;
	FILE * saFP = NULL;

	int result, total_resolved, total_records, m, q, pos;

	// open specified input file with ranks updated
	// after previous iteration - with 2 file pointers
	sprintf (file_name, "%s/ranks_%d", rank_dir, chunk_id);
	OpenBinaryFileReadWrite (&current_FP, file_name);
	total_records = fread (buffer_current, sizeof(long), WORKING_CHUNK_SIZE, current_FP);

	//handle reading next_rank
	if (next_chunk_dist) {
		sprintf (file_name, "%s/ranks_%d", rank_dir, chunk_id+next_chunk_dist);
		OpenBinaryFileRead (&next_FP, file_name);
		fread (buffer_next, sizeof (long), WORKING_CHUNK_SIZE, next_FP);
	} else{
		sprintf (file_name, "%s/ranks_%d", rank_dir, chunk_id);
		OpenBinaryFileRead (&next_FP, file_name);
		if(fseek(next_FP, (1 << h)*sizeof(long), SEEK_SET)) {
			printf ("Fseek failed trying to move to position %d in ranks file\n", (1 << h));
			exit (1);
		}
		int r = fread (buffer_next, sizeof (long), WORKING_CHUNK_SIZE, next_FP);
		// fclose(next_FP);
		if (chunk_id+1 < total_chunks) {
			fclose(next_FP);
			sprintf (file_name, "%s/ranks_%d", rank_dir, chunk_id+1);
			OpenBinaryFileRead (&next_FP, file_name);
			fread (buffer_next + r, sizeof (long), (1<<h), next_FP);
		}
	}

	// open local suffix array file where suffixes are sorted
	// according to local current,next for this iteration
	sprintf (file_name, "%s/sa_%d", rank_dir, chunk_id);
	OpenBinaryFileRead (&saFP, file_name);
	//read elements of sa - as many as there are ranks
	result = fread (sa_buffer, sizeof(int), total_records, saFP);
	if(result != total_records) {
		printf("Could not read %d elements of suffix array: only %d\n", total_records, result);
		return FAILURE;
	}
	//open file with global updates - if exists
	sprintf (file_name, "%s/global_%d", temp_dir, chunk_id);

	OpenBinaryFileRead(&global_resolved_FP, file_name);
	//read content of global resolved triples(curr, next, new) into buffer
	fseek (global_resolved_FP, 0, SEEK_END);
	total_resolved = ftell (global_resolved_FP)/sizeof(long);
	rewind(global_resolved_FP);
	if (total_resolved == 0) {
		return EMPTY;
	}

	//read all global updates for this chunk
	// (sorted by curr,next rank) into an array updated_ranks
	result = fread (updated_ranks, sizeof (long), total_resolved, global_resolved_FP);
	if (result != total_resolved) {
		printf ("Error reading global resolved ranks file %s: wanted to read %d but fread returned %d\n", file_name, total_resolved,result);
		return FAILURE;
	}
	fclose (global_resolved_FP);

	//read specified number of total records in this interval
	//read as many elements of next array as are available


	m = 0;
	q = 0;
	//position in local sa
	long curr, next;
	//printf("%d\n", total_resolved);
	while (q < total_resolved) {
		pos = sa_buffer [m];
		curr = buffer_current[pos];
		// if (pos == 298373 && chunk_id == 1){
		// 	printf("pos=%d, m=%d, q=%d, updated=%ld\n", pos, m, q, updated_ranks[q]);
		// }
		while (curr <= 0) {

			m++;
			pos = sa_buffer [m];
			curr = buffer_current[pos];
			// if (pos == 298373 && chunk_id == 1){
			// 	printf("pos=%d, m=%d, q=%d, updated=%ld\n", pos, m, q, updated_ranks[q]);
			// }
		}

		next = buffer_next[pos];
		while (curr == buffer_current[pos] && next == buffer_next[pos] && m < total_records)
		{
			//printf("pos=%d, m=%d, q=%d, updated=%ld\n", pos, m, q, updated_ranks[q]);
			buffer_current[pos] = updated_ranks[q];
			m++;
			if (m < total_records) {
				pos = sa_buffer[m];
				// if (pos == 298373 && chunk_id == 1){
				// 	printf("pos=%d, m=%d, q=%d, updated=%ld\n", pos, m, q, updated_ranks[q]);
				// }
				//printf("curr=%ld, next=%ld\n", buffer_current[pos], buffer_next[pos]);
			}
		}
		q++;
	}

	//return pointer to the beginning of the chunk
	rewind(current_FP);
	Fwrite (buffer_current, sizeof(long), total_records, current_FP);

	fclose (current_FP);
	fclose (next_FP);
	fclose (saFP);

	return SUCCESS;
}

int main (int argc, char **argv){
	char * rank_dir;
	char * temp_dir;
	int chunk_id, total_chunks, h;

	if (argc<5) {
		puts ("Run ./update_local_ranks <local_ranks_dir> <temp_dir> <total_chunks> <h>");
		return FAILURE;
	}

	rank_dir = argv[1];
	temp_dir = argv[2];
	total_chunks = atoi(argv[3]);
	h = atoi(argv[4]);

	//allocate input buffers for current and next ranks, and for local suffix array
	long * buffer_current = (long *) Calloc (WORKING_CHUNK_SIZE*sizeof(long));
	long * buffer_next = (long *) Calloc (WORKING_CHUNK_SIZE*sizeof(long));
	int * sa_buffer = (int *) Calloc (WORKING_CHUNK_SIZE*sizeof(int));
	long * updated_ranks = (long *) Calloc (WORKING_CHUNK_SIZE*sizeof(long));

	int more_runs = EMPTY;
    for (chunk_id=0; chunk_id<total_chunks; chunk_id++) {
  	   int result = update_local_ranks (rank_dir, temp_dir, total_chunks, chunk_id, h, buffer_current, buffer_next, sa_buffer, updated_ranks);
       if (result == FAILURE){
         return FAILURE;
       }
       if (result != EMPTY){
         more_runs = SUCCESS;
       }
    }

	free (updated_ranks);
	free (buffer_current);
	free (buffer_next);
	free (sa_buffer);

	return more_runs;
}
