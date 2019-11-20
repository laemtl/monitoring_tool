#ifndef __FLOW_QUEUE_H__
#define __FLOW_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.h"
#include "flow.hpp"

class FlowQueue {
	public:
		pthread_mutex_t mutex;
        Flow *first;
        Flow *last;
        int qlen;

		FlowQueue();
		int enq(Flow *flow);
		Flow* deq();
		int clear();
		int len();
		void print();
};

#endif