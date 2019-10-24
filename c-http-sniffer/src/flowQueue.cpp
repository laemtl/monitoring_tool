#include "flowQueue.hpp"

FlowQueue::FlowQueue()
{
	pthread_mutex_init(&mutex, NULL);
	
	first = NULL;
	last = NULL;
	length = 0;
}

int FlowQueue::enq(Flow *flow)
{
	pthread_mutex_lock(&mutex);


	if (length == 0){
		first = flow;
		last = flow;
		last->next = NULL;
		length++;
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	last->next = flow;
	last = flow;
	last->next = NULL;
	length++;

	pthread_mutex_unlock(&mutex);
	return 0;
}

Flow* FlowQueue::deq(void)
{
	pthread_mutex_lock(&mutex);
	Flow *f = NULL;
	if(length == 0){
		pthread_mutex_unlock(&mutex);
		return NULL;
	}else if(length == 1){
		last = NULL;
	}

	f = first;

	if(first != NULL) {
		first = first->next;
		length--;
	}

	pthread_mutex_unlock(&mutex);
	return f;
}

int FlowQueue::clear(void)
{
	pthread_mutex_lock(&mutex);

	Flow *f;

	while(length > 0){
		f = first;
		first = first->next;
		delete f;
		length--;
	}
	first =  NULL;
	last = NULL;
	length = 0;

	pthread_mutex_unlock(&mutex);
	return 0;
}

int FlowQueue::getLength()
{
	return length;
}

void FlowQueue::print() {
	printf("(Flow queue length)%d\n", getLength());
}