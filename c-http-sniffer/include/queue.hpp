#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <pthread.h>

template <class E>
class NodeQ
{
    public:
        E* elem;
        NodeQ* next;
};

template <class E>
class Queue 
{
	public:
		unsigned int length;
    	NodeQ<E>* first;
    	NodeQ<E>* last;
   		pthread_mutex_t mutex;

		Queue();
		~Queue();
		bool enq(E* elem);
		E* deq();
		void clr();
		unsigned int getLength();
		void print();
};

#endif