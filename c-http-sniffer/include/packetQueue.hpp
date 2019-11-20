#ifndef __PACKET_QUEUE_H__
#define __PACKET_QUEUE_H__

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "packet.h"
#include "util.h"

class PacketQueue {
	public:
		unsigned int que_len;
        packet_t *pkt_first;
        packet_t *pkt_last;
        pthread_mutex_t mutex;	

		PacketQueue();
		bool enq(packet_t *pkt);
		packet_t * deq();
		bool clr();
		unsigned int len();
        void print();
};

#endif