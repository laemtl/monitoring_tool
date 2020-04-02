#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

class Qnode {
    public:
        void* elem;
        Qnode* next;
		Qnode(void* e);
		Qnode();
};

class Queue {
	public:
		pthread_mutex_t mutex;
        Qnode *first;
        Qnode *last;
        int qlen;

		Queue();
		void enq(void *elem);
		void* deq();
		void clear();
		int len();
		void print();
};

#include "util.h"

#endif