#include "queue.hpp"

template <class E>
Queue<E>::Queue() {
	first = NULL;
	last = NULL;
	length = 0;
	pthread_mutex_init(&mutex, NULL);
}

template <class E>
bool Queue<E>::enq(E* elem) {
    NodeQ<E>* node = new NodeQ<E>();
    node->elem = elem;
    node->next = NULL;

	pthread_mutex_lock(&mutex);
	if(length == 0) {
		first = node;
		last = node;
		length++;
		pthread_mutex_unlock(&mutex);
		return true;
	}
	last->next = node;
	last = node;
	length++;
	pthread_mutex_unlock(&mutex);

	return true;
}

template <class E>
E* Queue<E>::deq() {
    E* elem;
    NodeQ<E>* node;

    pthread_mutex_lock(&mutex);
	if(length == 0) {
		pthread_mutex_unlock(&mutex);
		return NULL;
	} else if (length == 1) {
		last = NULL;
	}

	elem = first->elem;
    node = first;

	first = first->next;
	length--;
	pthread_mutex_unlock(&mutex);

    delete node;
	return elem;
}

template <class E>
void Queue<E>::clr() {
	pthread_mutex_lock(&mutex);
	while(length > 0) {        
        delete first->elem;
        delete first;
        first = first->next;
		length--;
	}

	first = NULL;
	last = NULL;
	length = 0;
	pthread_mutex_unlock(&mutex);
}

template <class E>
unsigned int Queue<E>::getLength() {
	return length;
}

template <class E>
void Queue<E>::print() {
	printf("(Queue length)%d\n", getLength());
}