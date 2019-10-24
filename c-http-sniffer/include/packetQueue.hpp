#ifndef __PACKETQUEUE_H__
#define __PACKETQUEUE_H__

#include <pthread.h>
#include <iostream>
#include "packet.hpp"

using namespace std; 

class PacketQueue
{
	unsigned int que_len;
    Packet *pkt_first;
    Packet *pkt_last;
    pthread_mutex_t pkt_queue_mutex;
	
	public:
		PacketQueue();
		void enq(Packet *pkt);
		Packet* deq();
		void clr();
		unsigned int len();
		void print();
};

#endif