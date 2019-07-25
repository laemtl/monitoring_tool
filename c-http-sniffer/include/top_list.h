#ifndef __TOPLIST_H__
#define __TOPLIST_H__

#include <pthread.h>
#define TL_SIZE	5

typedef struct _top_list top_list;
struct _top_list {
	void** list;
    pthread_mutex_t mutex;
    int size;
    int count;
};

#endif
