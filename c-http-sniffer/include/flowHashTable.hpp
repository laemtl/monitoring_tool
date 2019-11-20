#ifndef __FLOW_HASH_TABLE_H__
#define __FLOW_HASH_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "flow.hpp"
#include "util.h"
#include "packet.h"
#include "queue.hpp"

#define HASH_SIZE	13200

#define HASH_FLOW(flow_socket) ( \
( (flow_socket.sport & 0xff) | ((flow_socket.dport & 0xff) << 8) | \
  ((flow_socket.saddr & 0xff) << 16) | ((flow_socket.daddr & 0xff) << 24) \
) % HASH_SIZE)

class FlowHashTable {
	public:
		hash_mb_t *flow_hash_table[HASH_SIZE];
        int flow_cnt;	/* flows live in hash table */
		Queue* fq;

		FlowHashTable(Queue* fq);
		Flow* create(flow_s s);
		Flow* remove(Flow *f);
		Flow* find(flow_s s);
		int add_packet(packet_t *packet);
		int clear();
		void reset();
		int size();
		int fcnt();
		int scnt();
		int flow_scrubber(const int timeout);
		void print();
};

#endif