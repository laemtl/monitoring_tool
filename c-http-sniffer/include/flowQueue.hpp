#ifndef __FLOWQUEUE_H__
#define __FLOWQUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.hpp"
#include "flow.hpp"

/************************
 * flow queue functions *
 ************************/

class Flow;

class FlowQueue 
{
	private:
		pthread_mutex_t mutex;
        Flow* first;
        Flow* last;
        int length;	/* flow queue length */
	public:
		FlowQueue();
		int enq(Flow *flow);
		Flow* deq();
		int clear();
		int getLength();
		void print();
};

#endif