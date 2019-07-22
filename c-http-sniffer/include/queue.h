#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>
#include "util.h"

typedef struct _q_node Q_Node;
struct _q_node {
    void* elem;
    void* next;
};

typedef struct _queue Queue;
struct _queue {
    unsigned int len;
    Q_Node* first;
    Q_Node* last;
    pthread_mutex_t mutex;
    void (*free)(void*);
};

BOOL queue_init(Queue* q);
BOOL queue_enq(Queue* q, void* elem);
void* queue_deq(Queue* q);
BOOL queue_clr(Queue* q);
unsigned int queue_len(Queue* q);
void queue_print(Queue* q);

#endif