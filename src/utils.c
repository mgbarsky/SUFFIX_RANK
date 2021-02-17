#include "utils.h"
#include <errno.h>

void * Calloc (int num_bytes) {
	void * result =  calloc (num_bytes, 1);
	if (result == NULL) {
		printf ("Could not allocate array of size %d bytes\n", num_bytes);
		exit (1);
	}
	return result;
}


void OpenBinaryFileRead (FILE ** fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "rb" )))  {
		perror("Error:");
		printf("Could not open input binary file \"%s\" for reading \n", file_name);
		exit (1);
	}
}

void OpenBinaryFileReadWrite (FILE ** fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "r+b" ))) {
		printf("Could not open input binary file \"%s\" for reading and writing \n", file_name);
		exit (1);
	}
}

void OpenBinaryFileWrite (FILE ** fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "wb" )))  {
		printf("Could not open output binary file \"%s\" for writing \n", file_name);
		exit (1);
	}
}


void OpenBinaryFileAppend (FILE **fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "ab" )))  {
		printf("%s\n", strerror(errno));
		printf("Could not open binary file \"%s\" for appending \n", file_name);
		exit (1);
	}
}

void Fwrite (const void *buffer, size_t elem_size, size_t num_elements, FILE *fp ) {
	int written = fwrite (buffer, elem_size, num_elements, fp);
	if (written != num_elements) {
		printf ("Failed to write %ld elements to file: fwrite returned %ld\n", (long)num_elements, (long)written);
		exit (1);
	}
}

void OpenFileWrite (FILE ** fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "w" )))   {
		printf("Could not open file \"%s\" for writing \n", file_name);
		exit (1);
	}
}

void OpenFileRead (FILE ** fp, char * file_name) {
	if(!(*fp= fopen ( file_name, "r" )))   {
		printf("Could not open file \"%s\" for reading \n", file_name);
		exit (1);
	}
}

//reference <citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.14.8162&rep=rep1&type=pdf> p1260
void tsort(int *sa, long *next_ranks, int n){
	int a, b, c, d, l, h, s, tmp;
	long v;
	if (n <= 1) return;
	if (n < 5) {
		int i;
		for (i = 1; i < n; i++) {
			int j = i;
			while (KEY(j) < KEY(j-1)) {
				SWAP(&sa[j], &sa[j-1]);
				j--;
				if (j==0) break;
			}
		}
		return;
	}
	v = KEY(n/2);
	if (n > 5) {
		long v1, v2;
		v1 = KEY(0);
		v2 = KEY(n-1);
		if (n > 40) {
			s = n/8;
			v1 = MED3(v1, KEY(s), KEY(2*s));
			v = MED3(KEY(n/2-s), v, KEY(n/2+s));
			v2 = MED3(KEY(n-1-2*s), KEY(n-1-s), v2);
		}
		v = MED3(v, v1, v2);
	}
	a = b = 0;
	c = d = n-1;
	for (;;) {
		while (b <= c && v >= KEY(b)) {
			if (v == KEY(b)) {
				SWAP(&sa[a], &sa[b]);
				a++;
			}
			b++;
		}
		while (c >= b && KEY(c) >= v) {
			if (v == KEY(c)) {
				SWAP(&sa[d], &sa[c]);
				d--;
			}
			c--;
		}
		if (b > c) break;
		SWAP(&sa[b], &sa[c]);
		b++;
		c--;
	}
	s = MIN(a, b-a);
	for(l = 0, h = b-s; s; s--) {
		SWAP(&sa[l], &sa[h]);
		l++;
		h++;
	}
	s = MIN(d-c, n-1-d);
	for(l = b, h = n-s; s; s--) {
		SWAP(&sa[l], &sa[h]);
		l++;
		h++;
	}
	tsort(sa, next_ranks, b-a);
	tsort(sa + n-(d-c), next_ranks,  d-c);
}
