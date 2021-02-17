#ifndef INIT_HASH_TABLE_H
#define INIT_HASH_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#define DEFAULT_HASH_SIZE 1001

typedef struct hash_node {
	unsigned int key;
	long count;
	long rank;
    struct hash_node *next;
}HNode;

HNode * lookup (HNode ** hash_table, int hash_size, unsigned int key);

HNode * insert (HNode ** hash_table, int hash_size, unsigned int key);

void free_hashtable (HNode ** hash_table, int hash_size);
void print_table (HNode ** hash_table, int hash_size);
void print_linked_list(HNode* head);
#endif
