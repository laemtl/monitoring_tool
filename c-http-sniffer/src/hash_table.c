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
int hash_init(hash_t* ht, int size /*, int type*/) {
	if(ht == NULL) return 1;

	int ret, i;
    ht->size = size;
    //ht->type = type;
    ht->cnt = 0;
	
    ht->buckets = MALLOC(hash_mb_t2*, size);
    for(i=0; i<size; i++){
		ht->buckets[i] = MALLOC(hash_mb_t2, 1);
       	hash_mb_t2* bucket = ht->buckets[i];

		bucket->first = NULL;
		bucket->last = NULL;
		bucket->elm_cnt = 0;
		ret = pthread_mutex_init(&(bucket->mutex), NULL);
		if (ret != 0) {
			return -1;
		}
	}
	return 0;
}


/* Create a new record in hash table */
node* hash_new(void *value, hash_t* ht) {
	int n1 = sizeof("aaa.bbb.ccc.ddd") + 1;
	char *saddr[n1];
	strncpy(saddr, ip_ntos(((Addr*)value)->ip), n1);
	saddr[n1] = '\0';
	printf("source infos: %s %" PRIu16 "\n", saddr, ((Addr*)value)->port);
				


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
	ht->cnt++;

	pthread_mutex_unlock(&(hm->mutex));
	return n;
}

/* Try to find a record in hash table */
node* hash_find(void *value, hash_t* ht) {
	if(ht == NULL || value == NULL) return 1;
	
	hash_mb_t *hm = NULL;
	node	*e = NULL;
	hm = ht->buckets[ht->hash_fn(value)];
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
	ht->cnt--;
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
        ht->update_fn(e->value);
	} else {
        e = hash_new(value, ht);
		ht->update_fn(e->value);
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int hash_clear(hash_t* ht) {
	if(ht == NULL) {
		printf("ht is NULL \n");
		return 1;
	}

	int i;
	
	for(i=0; i<ht->size; i++){
		pthread_mutex_lock(&(ht->buckets[i]->mutex));
	}

	node* elem = NULL;
	ht->cnt = 0;
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
	return ht->cnt;
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
