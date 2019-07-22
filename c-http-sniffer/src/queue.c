#include "queue.h"

BOOL queue_init(Queue q) {
	int ret;
	
	q.first = NULL;
	q.last = NULL;
	q.len = 0;
	ret = pthread_mutex_init(&q.mutex, NULL);
	if(ret != 0) return FALSE;

	return TRUE;
}

BOOL queue_enq(Queue q, void* elem) {
	pthread_mutex_lock(&q.mutex);
	if(q.len == 0)
	{
		q.first = elem;
		q.last = elem;
		q.last->next = NULL;
		q.len++;
		pthread_mutex_unlock(&q.mutex);
		return TRUE;
	}
	q.last->next = elem;
	q.last = elem;
	q.last->next = NULL;
	q.len++;
	pthread_mutex_unlock(&q.mutex);
	return TRUE;
}

void* queue_deq(Queue q) {

    pthread_mutex_lock(&q.mutex);
	void* elem;
	if(q.len == 0)
	{
		pthread_mutex_unlock(&q.mutex);
		return NULL;
	}
	else if (q.len == 1)
	{
		q.last = NULL;
	}
	elem = q.first;
	q.first = q.first->next;
	q.len--;
	pthread_mutex_unlock(&q.mutex);
	return elem;
}

BOOL queue_clr(Queue q) {
	pthread_mutex_lock(&(q.mutex));
	void *elem;
	while(q.len > 0)
	{
		elem = q.first;
		q.first = q.first->next;
		q.free(elem);
		q.len--;
	}
	q.first =  NULL;
	q.last = NULL;
	q.len = 0;
	pthread_mutex_unlock(&q.mutex);
	return TRUE;
}

unsigned int queue_len(Queue q) {
	return q.len;
}

void queue_print(Queue q) {
	printf("(Queue length)%d\n", queue_len(q));
}