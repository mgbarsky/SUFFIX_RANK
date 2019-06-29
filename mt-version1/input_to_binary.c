#include "utils.h"
/**
 * This will convert a file of characters into a binary file where each character is represented by its
 * integral value.
 * If the integral value of the character is negative, we will ignore this fact and consider it as
 * its absolute value
 * The arguments -
 *   input file name,
 *  temp output directory,
 *	file_id (number from 0 to num_files)
 *	whether consider each line separately
 * It will also add the alphabetically smallest value '0' at the end of the file
 **/
int main (int argc, char ** argv) {
	FILE * inputFP = NULL;
	FILE * outputFP = NULL;
	char *file_name;
	char * output_directory;
	int file_num;
	unsigned int *buffer;
	int pos_in_buffer = 0;
	char line [MAX_LINE];
	int line_len;
	char output_file_name [MAX_PATH_LENGTH];
	int i;
	long binary_size = 0;


	if (argc < 4 ) {
		//./input_to_binary dna/dnaread1.txt input 0 0
		printf ("To run input_to_binary <text file name> <output directory> <file id>\n");
		return 1;
	}

	file_name = argv[1];

	output_directory = argv [2];

	file_num = atoi (argv[3]);

	//we collect characters in buffer before flushing it to disk
	buffer = (unsigned int *) Calloc (DEFAULT_CHAR_BUFFER_SIZE * sizeof(unsigned int));

	//open text input for reading
	OpenFileRead (&inputFP, file_name);

	sprintf (output_file_name, "%s/binary_input", output_directory);
	OpenBinaryFileAppend (&outputFP, output_file_name);

	while (fgets (line, MAX_LINE-1, inputFP)!=NULL ) {
		line[strcspn(line, "\r\n")] = '\0';
		line_len = strlen(line);
		for (i=0; i< line_len; i++) {
			unsigned int valid_char = DEFAULT_CHAR;
			if (line[i] > 0 && (unsigned char)line[i] < 255)
				valid_char = (unsigned int)line[i];
			buffer[pos_in_buffer++] = valid_char;
			binary_size++;
			if (pos_in_buffer == DEFAULT_CHAR_BUFFER_SIZE) {
				Fwrite (buffer, sizeof(unsigned int), pos_in_buffer, outputFP);
				pos_in_buffer = 0;
			}
		}
	}

	fclose(inputFP);
	if (pos_in_buffer > (DEFAULT_CHAR_BUFFER_SIZE - file_num)) {
		Fwrite (buffer, sizeof(unsigned int), pos_in_buffer, outputFP);
		pos_in_buffer = 0;
	}

	//add sentinel - for the end-of-file
	for (i=0; i<file_num; i++) {
		buffer[pos_in_buffer++] = 0;
		binary_size++;
	}

	Fwrite (buffer, sizeof(unsigned int), pos_in_buffer, outputFP);

	fclose(outputFP);
	free(buffer);

	printf("Binary size: %ld\n", binary_size);

	return SUCCESS;
}
