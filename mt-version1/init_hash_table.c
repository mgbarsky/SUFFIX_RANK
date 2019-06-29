#include "init_hash_table.h"

/*
Hash values for different types
    Create a int result and assign a non-zero value.

    For every field f tested in the equals() method, calculate a hash code c by:
        If the field f is a boolean: calculate (f ? 0 : 1);
        If the field f is a byte, char, short or int: calculate (int)f;
        If the field f is a long: calculate (int)(f ^ (f >>> 32));
        If the field f is a float: calculate Float.floatToIntBits(f);
        If the field f is a double: calculate Double.doubleToLongBits(f) and handle the return value like every long value;
        If the field f is an object: Use the result of the hashCode() method or 0 if f == null;
        If the field f is an array: see every field as separate element and calculate the hash value in a recursive fashion and combine the values as described next.

    Combine the hash value c with result:

    result = 37 * result + c

    Return result

This should result in a proper distribution of hash values for most use situations.
*/

HNode * lookup (HNode ** hash_table, int hash_size, unsigned int key) {
	HNode * np;
	unsigned int hash_pos = key % hash_size;
	for (np = hash_table[hash_pos]; np !=NULL; np = np->next){
		if (np->key == key) {
            return np; //found
        }
	}
	return NULL; //not found
}

HNode * insert (HNode ** hash_table, int hash_size, unsigned int key) {
	HNode *np = lookup(hash_table, hash_size, key);
	if (np == NULL){
		unsigned int hash_pos = key % hash_size;

		np = (HNode *) Calloc (sizeof (*np));
		np->key = key;
		np->count = 1;

		//insert on top of the corresponding list
		np->next = hash_table [hash_pos];
		hash_table [hash_pos] = np;
		return np;
	}else{
		np->count += 1;
		return NULL;
	}
}

void free_hashtable (HNode ** hash_table, int hash_size) {
	int i;
	for (i=0; i<hash_size; i++) { //iterate over hash array
		if (hash_table [i]!=NULL) { //if there is an entry at position i
			HNode *head = hash_table[i]; //find the head of the linked list
			HNode *current = head;
			while (current != NULL) {
				HNode * temp = current;
				current = current->next;
				free (temp);
			}
			hash_table [i] = NULL;  //BUG fix
		}
	}
}

void print_linked_list(HNode* head) {
	HNode * curr = head;
	while (curr != NULL) {
		printf("(key:%u, count:%ld)->", curr->key, curr->count);
		curr = curr->next;
	}
	printf("\n");
}

void print_table (HNode ** hash_table, int hash_size) {
	int i;
	for (i=0; i<hash_size; i++) { //iterate over hash array
		if (hash_table [i]!=NULL) { //if there is an entry at position i
			print_linked_list(hash_table[i]); //find the head of the linked list
		}
	}
}
