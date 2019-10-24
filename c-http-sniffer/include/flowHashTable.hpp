#ifndef __FLOW_HTABLE_H__
#define __FLOW_HTABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.hpp"
#include "packet.hpp"
#include "flowHashTable.hpp"
#include "hashTable.hpp"
#include "flow.hpp"
#include "tcpHeader.hpp"
#include "flowQueue.hpp"
#include "flowSocket.hpp"

// TODO: extend hashTable

class Flow;
class FlowQueue;

// flow.hash(int size)

class HashMb
{
	public:
		Flow	*first;
		Flow	*last;
		pthread_mutex_t mutex;
		int		elm_cnt;

		HashMb();
};

class FlowHashTable
{
private:
	const int HASH_INIT_SIZE = 13200;
	//Analysis analysis;
	int flow_cnt; /* flows live in hash table */
	FlowQueue* fq;
	int size;

public:
	vector<HashMb*> flowHashTable;	
	FlowHashTable(FlowQueue* fq);
	Flow* add(FlowSocket s);
	Flow* remove(Flow *f);
	Flow* find(FlowSocket s);
	int add_packet(Packet *packet);
	int clear();
	void reset();
	int getSize();
	int getFlowCnt();
	int getSlotCnt();
	int flow_scrubber(const int timeout);
	void print(void);

	~FlowHashTable();
};

#endif
