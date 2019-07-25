#ifndef ATTR_H
#define ATTR_H

#include "pthread.h"

typedef struct _attr Attr;
struct _attr {
	void* elem;
	int	req_tot;
	int stamp;
    pthread_mutex_t mutex;
};

#include "hash_table.h"

int req_compare(Attr* a1, Attr* a2);
void update_attr(Attr* a, hash_t* ht);

#endif