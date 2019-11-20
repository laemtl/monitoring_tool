#include "queue.hpp"

Qnode::Qnode(void* e) : next(NULL), elem(e) {}

Qnode::Qnode() : next(NULL), elem(NULL) {}

Queue::Queue() : first(NULL), last(NULL), qlen(0) {
    pthread_mutex_init(&mutex, NULL);
}

void Queue::enq(void *elem) {
    Qnode* node = new Qnode(elem);
    
    pthread_mutex_lock(&mutex);

    if (qlen == 0){
        first = node;
        last = node;
        qlen++;
        pthread_mutex_unlock(&mutex);
        return;
    }

    last->next = node;
    last = node;
    qlen++;

    pthread_mutex_unlock(&mutex);
}

void* Queue::deq() {
    void* elem = NULL;
	Qnode* node;

    pthread_mutex_lock(&mutex);
    if(qlen == 0){
        pthread_mutex_unlock(&mutex);
        return NULL;
    }else if(qlen == 1){
        last = NULL;
    }

    if(first != NULL) {
    	node = first;
        elem = first->elem;

        first = first->next;
        qlen--;
    }

    pthread_mutex_unlock(&mutex);
    return elem;
}

void Queue::clear() {
    pthread_mutex_lock(&mutex);

    Qnode *node;

    while(qlen > 0){
        node = first;
        first = first->next;
        delete node->elem;
        delete node;
        qlen--;
    }
    first =  NULL;
    last = NULL;
    qlen = 0;

    pthread_mutex_unlock(&mutex);
}

int Queue::len() {
    return qlen;
}

void Queue::print() {
    printf("(Queue length)%d\n", len());
}