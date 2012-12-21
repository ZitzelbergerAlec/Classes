/* David Merrick
 * 3/3/12
 * Development Environment: GCC (flip) with Notepad++
 */

#include <assert.h>
#include <string.h> //for strcmp function

#include <stdlib.h>
#include "hashMap.h"

int stringHash1(char * str){
    int i;
    int r = 0;
    for (i = 0; str[i] != '\0'; i++)
        r += str[i];
    return r;
}

int stringHash2(char * str){
    int i;
    int r = 0;
    for (i = 0; str[i] != '\0'; i++)
        r += (i+1) * str[i]; /*the difference between 1 and 2 is on this line*/
    return r;
}

void initMap (struct hashMap * ht, int tableSize)
{
    int index;
    
    if(ht == NULL)
        return;
    ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
    ht->tableSize = tableSize;
    ht->count = 0;
    for(index = 0; index < tableSize; index++)
        ht->table[index] = NULL;
}

void freeMap (struct hashMap * ht){
	struct hashLink* next;
    struct hashLink* cur;
	for(int i=0; i < ht->tableSize; i++){
		cur = ht->table[i];
		while(cur !=0){
			next = cur->next;
			free(cur);
			cur = next;
		}
		ht->table[i] = 0;
	}
	free(ht->table);
	ht->tableSize = 0;
	ht->count = 0;
			
}

void insertMap (struct hashMap * ht, KeyType k, ValueType v){
	int idx;
	struct hashLink* new = malloc(sizeof(struct hashLink));
	//initialize new link
	assert(new);
	new->next = 0;
	new->key = k;
	new->value = v;
	
	//check to see if map contains key. If so, remove it.
	if(containsKey(ht, k)){
		removeKey(ht, k);
	}
		
	idx = stringHash1(k) % ht->tableSize;
	
	if(idx < 0){
		idx += ht->tableSize;
	}
	if(!ht->table[idx]){
		ht->table[idx] = new;
	} else {
		struct hashLink* cur = ht->table[idx];
		while(cur->next){
			cur = cur->next;
		}
		cur->next = new;
	}
	ht->count++;
}

ValueType* atMap (struct hashMap * ht, KeyType k){
    int idx = stringHash1(k) % ht->tableSize;
	if(idx < 0){
		idx += ht->tableSize;
	}
	assert(containsKey(ht, k));
	struct hashLink* cur = ht->table[idx];
	
	while(strcmp(cur->key, k) != 0){ //strcmp returns 0 if the strings are equal
		cur = cur->next;
	}
    return &cur->value;
}

int containsKey (struct hashMap * ht, KeyType k){
    int idx = stringHash1(k) % ht->tableSize;
	if(idx < 0){
		idx += ht->tableSize;
	}
	
	struct hashLink* cur = ht->table[idx];
	
	while(cur){
		if(strcmp(cur->key, k) == 0){
			return 1;
		}
		cur = cur->next;
	}
    return 0;
}

void removeKey (struct hashMap * ht, KeyType k){
	struct hashLink* cur;
	struct hashLink* pre = NULL;
	
	int idx = stringHash1(k) % ht->tableSize;
	if(idx < 0){
		idx += ht->tableSize;
	}
	cur = ht->table[idx];
	while(strcmp(cur->key, k) != 0){
		pre = cur;
		cur = cur->next;
	}
	if(pre){
		pre->next = cur->next;
	} else {
		ht->table[idx] = 0;
	}
	free(cur);
}

int size(struct hashMap *ht){
	assert(ht);
    return ht->count;
}

int capacity(struct hashMap *ht){
    assert(ht);
    return ht->tableSize;
}

int emptyBuckets(struct hashMap *ht){
	int numEmptyBuckets = 0;
	for(int i = 0; i < ht->tableSize; i++){
		if(!ht->table[i]){
			numEmptyBuckets++;
		}
	}
    return numEmptyBuckets;
}

float tableLoad(struct hashMap *ht){
    assert(ht);
    return ht->count / (double) ht->tableSize;
}
