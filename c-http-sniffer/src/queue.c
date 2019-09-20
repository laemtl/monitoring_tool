#include <stdio.h>
#include <pthread.h>
#include "queue.h"

BOOL queue_init(Queue* q) {
	int ret;
	
	q->first = NULL;
	q->last = NULL;
	q->len = 0;
	ret = pthread_mutex_init(&q->mutex, NULL);
	if(ret != 0) return FALSE;

	return TRUE;
}

BOOL queue_enq(Queue* q, void* elem) {
    Q_Node* node = MALLOC(Q_Node, 1);
    node->elem = elem;
    node->next = NULL;

	pthread_mutex_lock(&q->mutex);
	if(q->len == 0) {
		q->first = node;
		q->last = node;
		q->len++;
		pthread_mutex_unlock(&q->mutex);
		return TRUE;
	}
	q->last->next = node;
	q->last = node;
	q->len++;
	pthread_mutex_unlock(&q->mutex);

	return TRUE;
}

void* queue_deq(Queue* q) {
    void* elem;
    Q_Node* node;

    pthread_mutex_lock(&q->mutex);
	if(q->len == 0) {
		pthread_mutex_unlock(&q->mutex);
		return NULL;
	} else if (q->len == 1) {
		q->last = NULL;
	}

	elem = q->first->elem;
    node = q->first;

	q->first = q->first->next;
	q->len--;
	pthread_mutex_unlock(&q->mutex);

    free(node);
	return elem;
}

BOOL queue_clr(Queue* q) {
	pthread_mutex_lock(&(q->mutex));
	while(q->len > 0) {        
        q->free(q->first->elem);
        free(q->first);
        q->first = q->first->next;
		q->len--;
	}

	q->first = NULL;
	q->last = NULL;
	q->len = 0;
	pthread_mutex_unlock(&q->mutex);
	return TRUE;
}

unsigned int queue_len(Queue* q) {
	return q->len;
}

void queue_print(Queue* q) {
	printf("(Queue length)%d\n", queue_len(q));
}