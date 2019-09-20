#ifndef ATTR_H
#define ATTR_H

#include "pthread.h"

typedef struct _attr Attr;
struct _attr
{
	void *elem;
	int cnt;
	int stamp;
	pthread_mutex_t mutex;
};

#include "hash_table.h"

Attr *create_attr(void *e);
void delete_attr(Attr *a);
void update_attr(Attr *a, hash_t *ht);
void add_attr(void *e, hash_t *ht);
int req_compare(Attr *a1, Attr *a2);

#endif