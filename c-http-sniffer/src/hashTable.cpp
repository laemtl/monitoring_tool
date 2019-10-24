/*
 * hash_table.c
 *
 * Created on: JUne 26, 2019
 * Author: Laetitia Fesselier
 */
//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//#include <string.h>
//#include <assert.h>
//#include <sys/time.h>
//#include <unistd.h>
//#include <inttypes.h>

#include "hashTable.hpp"

Bucket::Bucket()
	: first(NULL), last(NULL), elm_cnt(0) 
{
	pthread_mutex_init(&mutex, NULL);
}

HashTable::HashTable() 
	: size(HASH_INIT_SIZE)
{
	pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i<size; i++){
		hashTable.push_back(new Bucket());
	}
}

/* Create a new record in hash table */
NodeHT* HashTable::add(Hashable *value) {
	if(value == NULL) error("value is NULL \n");
	
	NodeHT* n = new NodeHT();
	n->elem = value;
	Bucket *b = NULL;
	b = hashTable[value->hash()];

	pthread_mutex_lock(&(b->mutex));

	if(b->elm_cnt == 0 ){
		n->next = NULL;
		n->prev = NULL;
		b->first = n;
	} else {
		b->last->next = n;
		n->prev = b->last;
	}
	b->last = n;
	b->last->next = NULL;
	n->bucket = b;
	b->elm_cnt++;
	tot_cnt++;
	int_cnt++;

	pthread_mutex_unlock(&(b->mutex));
	return n;
}

/* Try to find a record in hash table */
NodeHT* HashTable::find(Hashable* value) {
	if(value == NULL) error("value is NULL \n");
	
	Bucket *b = NULL;
	NodeHT	*e = NULL;

	u_int32_t key = value->hash();

	b = hashTable[key];
	pthread_mutex_lock(&(b->mutex));

	if (b->elm_cnt > 0){
		e = b->first;
		while(e != NULL){
			if(value->compare(e->elem) == 0){
				pthread_mutex_unlock(&(b->mutex));
				return e;
			} else {
				e = e->next;
				continue;
			}
		}
	}
	pthread_mutex_unlock(&(b->mutex));
	return NULL;
}

/* Delete a record in hash table */
NodeHT* HashTable::remove(Hashable* value) {
    if(value == NULL) error("value is NULL \n");
		
	NodeHT* n = find(value);
	if(n == NULL) return NULL;

	Bucket *b = n->bucket;

	if( b->elm_cnt == 1 && n == b->first){
		b->first = NULL;
		b->last = NULL;
	} else {
		if(n->prev == NULL){	/* the first record */
			b->first = n->next;
			b->first->prev = NULL;
		} else if(n->next == NULL) {
			b->last = n->prev;
			b->last->next = NULL;
		} else {
			n->prev->next = n->next;
			n->next->prev = n->prev;
		}
	}
	n->next = NULL;
	n->prev = NULL;
	b->elm_cnt--;
	tot_cnt--;
	return n;
}

/*
 * Add an element to hash table
 * Update the element if the element already exists;
 * Otherwise, make a new record and add the value to it.
 */
int HashTable::insert(Hashable* value) {
	if(value == NULL) error("value is NULL \n");
		
	NodeHT *e = NULL;
	e = find(value);
	if(e != NULL) {
		delete value;	
		e->elem->update();
	} else {
		e = add(value);
		e->elem->update();
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int HashTable::clear() {
	for(int i=0; i<size; i++){
		pthread_mutex_lock(&(hashTable[i]->mutex));
	}

	NodeHT* elem = NULL;
	tot_cnt = 0;
	int_cnt = 0;
	for(int i=0; i<size; i++){
		while(hashTable[i]->first != NULL ){
			elem = hashTable[i]->first;
			hashTable[i]->first = hashTable[i]->first->next;
			delete elem;
			hashTable[i]->elm_cnt--;
		}
		hashTable[i]->first = NULL;
		hashTable[i]->last = NULL;
	}

	for(int i=0; i<size; i++){
		pthread_mutex_unlock(&(hashTable[i]->mutex));
	}

	return 0;
}

/*void HashTable::reset() {
	for(int i=0; i<size; i++){
		delete buckets[i];
	}
	free(buckets);

	//tl_delete(&(ht->tl));
}*/

/* Return the size of hash table */
int HashTable::getSize() {
	return size;
}

/* Return the flow count of hash table */
int HashTable::getCnt() {
	return tot_cnt;
}

/* Return the count of hash slots consumed */
int HashTable::getSlotCnt() {
	int i = 0;
	int slot_consumed = 0;
	
	for(i=0; i<size; i++)
	{
		pthread_mutex_lock(&(hashTable[i]->mutex));
	}
	
	for(i=0; i<size; i++)
	{
		if(hashTable[i]->first != NULL)
		{
			slot_consumed++;
		}
	}
	
	for(i=0; i<size; i++)
	{
		pthread_mutex_unlock(&(hashTable[i]->mutex));
	}
	return slot_consumed;
}

/* Print hash table details for debugging */
void HashTable::print() {
	printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", getSize(), getSlotCnt(), getCnt() );
}

/*void reset_int_cnt(hash_t* ht) {
    pthread_mutex_lock(&(ht->mutex));
	ht->int_cnt = 0;
	pthread_mutex_unlock(&(ht->mutex));
}*/