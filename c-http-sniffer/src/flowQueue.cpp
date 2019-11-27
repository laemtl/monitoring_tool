#include "flowQueue.hpp"

FlowQueue::FlowQueue() : first(NULL), last(NULL), qlen(0) {
	pthread_mutex_init(&mutex, NULL);
}

int FlowQueue::enq(Flow *flow) {
	pthread_mutex_lock(&mutex);

	if (qlen == 0){
		first = flow;
		last = flow;
		last->next = NULL;
		qlen++;
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	last->next = flow;
	last = flow;
	last->next = NULL;
	qlen++;

	pthread_mutex_unlock(&mutex);
	return 0;
}

Flow* FlowQueue::deq() {
	pthread_mutex_lock(&mutex);
	Flow *f = NULL;
	if(qlen == 0){
		pthread_mutex_unlock(&mutex);
		return NULL;
	}else if(qlen == 1){
		last = NULL;
	}

	f = first;

	if(first != NULL) {
		first = first->next;
		qlen--;
	}

	pthread_mutex_unlock(&mutex);
	return f;
}

int FlowQueue::clear() {
	pthread_mutex_lock(&mutex);

	Flow *f;

	while(qlen > 0){
		f = first;
		first = first->next;
		delete f;
		qlen--;
	}
	first =  NULL;
	last = NULL;
	qlen = 0;

	pthread_mutex_unlock(&mutex);
	return 0;
}

int FlowQueue::len() {
	return qlen;
}

void FlowQueue::print() {
	printf("(Flow queue length)%d\n", len());
}
