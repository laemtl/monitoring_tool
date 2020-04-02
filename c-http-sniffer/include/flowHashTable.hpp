#ifndef __FLOW_HASH_TABLE_H__
#define __FLOW_HASH_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#define HASH_SIZE	13200

#define HASH_FLOW(flow_socket) ( \
( (flow_socket.sport & 0xff) | ((flow_socket.dport & 0xff) << 8) | \
  ((flow_socket.saddr & 0xff) << 16) | ((flow_socket.daddr & 0xff) << 24) \
) % HASH_SIZE)

class Flow;
class Queue;
struct _flow_s;
typedef struct _flow_s	flow_s;
typedef struct _packet_t packet_t;

/**
 * Hash management block
 */
typedef struct _hash_mb_t hash_mb_t;
struct _hash_mb_t
{
	Flow	*first;
	Flow	*last;
	pthread_mutex_t mutex;
	int		elm_cnt;
};

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

#include "flow.hpp"
#include "queue.hpp"
#include "util.h"
#include "packet.h"

#endif