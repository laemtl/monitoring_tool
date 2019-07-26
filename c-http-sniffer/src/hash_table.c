/*
 * hash_table.c
 *
 * Created on: JUne 26, 2019
 * Author: Laetitia Fesselier
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <inttypes.h>

#include "util.h"
#include "data.h"
#include "hash_table.h"

/* Initiate the hash table with no elems */
int hash_init(hash_t* ht, int (*hash_fn)(void*, void*), int (*compare_fn)(void*, void*), int (*update_fn)(void*, void*)) {
	int ret, i;
	if(ht == NULL) return 1;

	ret = pthread_mutex_init(&(ht->mutex), NULL);
	if (ret != 0) return -1;

	ht->size = HASH_SIZE;
	
    ht->buckets = MALLOC(hash_mb_t2*, ht->size);
    for(i=0; i<ht->size; i++){
		ht->buckets[i] = MALLOC(hash_mb_t2, 1);
		ht->buckets[i]->first = NULL;
		ht->buckets[i]->last = NULL;
		ht->buckets[i]->elm_cnt = 0;
		ret = pthread_mutex_init(&(ht->buckets[i]->mutex), NULL);
		if (ret != 0) return -1;
	}

	ht->hash_fn = hash_fn;
    ht->compare_fn = compare_fn;
    ht->update_fn = update_fn;

	tl_init(&(ht->tl));

	return 0;
}


/* Create a new record in hash table */
node* hash_new(void *value, hash_t* ht) {
	if(ht == NULL || value == NULL) return 1;
	
	node* n = MALLOC(node, 1);
	n->value = value;
	hash_mb_t2 *hm = NULL;
	hm = ht->buckets[ht->hash_fn(value)];

	pthread_mutex_lock(&(hm->mutex));

	if(hm->elm_cnt == 0 ){
		n->next = NULL;
		n->prev = NULL;
		hm->first = n;
	} else {
		hm->last->next = n;
		n->prev = hm->last;
	}
	hm->last = n;
	hm->last->next = NULL;
	n->hmb = hm;
	hm->elm_cnt++;
	ht->tot_cnt++;
	ht->int_cnt++;

	pthread_mutex_unlock(&(hm->mutex));
	return n;
}

/* Try to find a record in hash table */
node* hash_find(void *value, hash_t* ht) {
	if(ht == NULL || value == NULL) return 1;
	
	hash_mb_t *hm = NULL;
	node	*e = NULL;

	int key = ht->hash_fn(value);
	hm = ht->buckets[key];
	pthread_mutex_lock(&(hm->mutex));

	if (hm->elm_cnt > 0){
		e = hm->first;
		while(e != NULL){
			if(ht->compare_fn(value, e->value) == 0){
				pthread_mutex_unlock(&(hm->mutex));
				return e;
			} else {
				e = e->next;
				continue;
			}
		}
	}
	pthread_mutex_unlock(&(hm->mutex));
	return NULL;
}

/* Delete a record in hash table */
node* hash_delete(void *value, hash_t* ht) {
    if(ht == NULL || value == NULL) return 1;
		
	node* n = hash_find(value, ht);
	if(n == NULL) return NULL;

	hash_mb_t *hm = n->hmb;

	if( hm->elm_cnt == 1 && n == hm->first){
		hm->first = NULL;
		hm->last = NULL;
	} else {
		if(n->prev == NULL){	/* the first record */
			hm->first = n->next;
			hm->first->prev = NULL;
		} else if(n->next == NULL) {
			hm->last = n->prev;
			hm->last->next = NULL;
		} else {
			n->prev->next = n->next;
			n->next->prev = n->prev;
		}
	}
	n->next = NULL;
	n->prev = NULL;
	hm->elm_cnt--;
	ht->tot_cnt--;
	ht->int_cnt--;
	return n;
}

/*
 * Add an element to hash table
 * Update the element if the element already exists;
 * Otherwise, make a new record and add the value to it.
 */

int hash_add(void *value, hash_t* ht) {
	if(ht == NULL || value == NULL) return 1;
		
	node *e = NULL;

	e = hash_find(value, ht);
	if(e != NULL) {
		free(value);	
		ht->update_fn(e->value, ht);
	} else {
		Attr* attr = (Attr*)value;
		e = hash_new(value, ht);
		ht->update_fn(e->value, ht);
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int hash_clear(hash_t* ht) {
	if(ht == NULL) {
		return 1;
	}

	int i;
	
	for(i=0; i<ht->size; i++){
		pthread_mutex_lock(&(ht->buckets[i]->mutex));
	}

	node* elem = NULL;
	ht->tot_cnt = 0;
	ht->int_cnt = 0;
	for(i=0; i<ht->size; i++){
		while(ht->buckets[i]->first != NULL ){
			elem = ht->buckets[i]->first;
			ht->buckets[i]->first = ht->buckets[i]->first->next;
			free(elem->value);
			free(elem);
			ht->buckets[i]->elm_cnt--;
		}
		ht->buckets[i]->first = NULL;
		ht->buckets[i]->last = NULL;
	}

	for(i=0; i<ht->size; i++){
		pthread_mutex_unlock(&(ht->buckets[i]->mutex));
	}

	return 0;
}

/* Return the size of hash table */
int hash_size(hash_t* ht) {
	if(ht == NULL) return 1;
	return ht->size;
}

/* Return the flow count of hash table */
int hash_cnt(hash_t* ht) {
	if(ht == NULL) return 1;
	return ht->tot_cnt;
}

/* Return the count of hash slots consumed */
int hash_scnt(hash_t* ht) {
	if(ht == NULL) return 1;
	int i = 0;
	int slot_consumed = 0;
	
	for(i=0; i<ht->size; i++)
	{
		pthread_mutex_lock(&(ht->buckets[i]->mutex));
	}
	
	for(i=0; i<ht->size; i++)
	{
		if(ht->buckets[i]->first != NULL)
		{
			slot_consumed++;
		}
	}
	
	for(i=0; i<ht->size; i++)
	{
		pthread_mutex_unlock(&(ht->buckets[i]->mutex));
	}
	return slot_consumed;
}

/* Print hash table details for debugging */
void hash_print(hash_t* ht) {
	if(ht == NULL) return 1;
	printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", hash_size(ht), hash_scnt(ht), hash_cnt(ht) );
}

void reset_int_cnt(hash_t* ht) {
    pthread_mutex_lock(&(ht->mutex));
	ht->int_cnt = 0;
	pthread_mutex_unlock(&(ht->mutex));
}