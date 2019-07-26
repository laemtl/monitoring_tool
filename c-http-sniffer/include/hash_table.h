

#include <pthread.h>
#include "top_list.h"

#ifndef HTABLE_H
#define HTABLE_H

#define HASH_SIZE	13200

typedef struct _node node;

/**
 * Hash management block
 */
typedef struct _hash_mb_t2 hash_mb_t2;
struct _hash_mb_t2 {
	node	*first;
	node	*last;
	pthread_mutex_t mutex;
	int		elm_cnt;
};

struct _node {
	node		*next;
	node		*prev;
    hash_mb_t2	*hmb;
    void        *value;
};

// memset to 0 on init
typedef struct _hash_t hash_t;
struct _hash_t {
    pthread_mutex_t mutex;

    // number of buckets in the ht
    int size;
    // total number of item in the hash table
    int tot_cnt;
    // total number of updated item in the hash table
    // within the current interval (1 sec for rate computation)
    int int_cnt;

    hash_mb_t2** buckets;
    top_list tl;

    int (*hash_fn)(void*);
    int (*compare_fn)(void*, void*);
    void (*update_fn)(void*, void*);
};

int hash_init(hash_t* ht, int (*hash_fn)(void*, void*), int (*compare_fn)(void*, void*), int (*update_fn)(void*, void*));
node* hash_new(void *value, hash_t* ht);
node* hash_find(void *value, hash_t* ht);
node* hash_delete(void *value, hash_t* ht);
int hash_add(void *value, hash_t* ht);
int hash_clear(hash_t* ht);
int hash_size(hash_t* ht);
int hash_cnt(hash_t* ht);
int hash_scnt(hash_t* ht);
void hash_print(hash_t* ht);

#endif