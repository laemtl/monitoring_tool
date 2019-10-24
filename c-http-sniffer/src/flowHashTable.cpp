#include "flowHashTable.hpp"

HashMb::HashMb() 
	: first(NULL), last(NULL), elm_cnt(0)
{
	pthread_mutex_init(&mutex, NULL);
}

FlowHashTable::FlowHashTable(FlowQueue* fq)
	: fq(fq), size(HASH_INIT_SIZE), flow_cnt(0)
{
	/* Initiate the flow hash table with no flows */
	for(int i=0; i<size; i++){
		flowHashTable.push_back(new HashMb());
	}
}

FlowHashTable::~FlowHashTable()
{
}

/* Create a new record in hash table according to flow's socket */
Flow* FlowHashTable::add(FlowSocket s)
{
	Flow *f = NULL;
	HashMb *hm = NULL;

	f = new Flow(this, fq);

	f->socket.saddr = s.saddr;
	f->socket.daddr = s.daddr;
	f->socket.sport = s.sport;
	f->socket.dport = s.dport;

	hm = flowHashTable[s.hash(size)];
	pthread_mutex_lock(&(hm->mutex));

	if(hm->elm_cnt == 0 ){
		f->next = NULL;
		f->prev = NULL;
		hm->first = f;
	}else{
		hm->last->next = f;
		f->prev = hm->last;
	}
	hm->last = f;
	hm->last->next = NULL;
	f->hmb = hm;
	hm->elm_cnt++;
	flow_cnt++;

	pthread_mutex_unlock(&(hm->mutex));
	return f;
}

/* Delete a flow record in hash table */
Flow* FlowHashTable::remove(Flow *f)
{
	HashMb *hm = NULL;
	hm = f->hmb;
	if( hm->elm_cnt == 1 && f == hm->first){
		hm->first = NULL;
		hm->last = NULL;
	}else{
		if(f->prev == NULL){	/* the first flow record */
			hm->first = f->next;
			hm->first->prev = NULL;
		}else if(f->next == NULL){
			hm->last = f->prev;
			hm->last->next = NULL;
		}else{
			f->prev->next = f->next;
			f->next->prev = f->prev;
		}
	}
	f->next = NULL;
	f->prev = NULL;
	hm->elm_cnt--;
	flow_cnt--;
	return f;
}

/* Try to find a flow record in hash table based on its socket */
Flow* FlowHashTable::find(FlowSocket s)
{
	HashMb *hm = NULL;
	Flow	*flow = NULL;
	hm = flowHashTable[s.hash(size)];
	pthread_mutex_lock(&(hm->mutex));

	if (hm->elm_cnt > 0){
		flow = hm->first;
		while(flow != NULL){
			if(s.compare(&flow->socket) == 0){
				pthread_mutex_unlock(&(hm->mutex));
				return flow;
			}else{
				flow = flow->next;
				continue;
			}
		}
	}
	pthread_mutex_unlock(&(hm->mutex));
	return NULL;
}

/*
 * Add a packet to hash table
 * Link the packet to flow's packet list if the flow has already existed;
 * Otherwise, make a new flow record and add the packet to it.
 */
int FlowHashTable::add_packet(Packet *packet)
{
	FlowSocket cs;
	Flow *f = NULL;

	cs.saddr = packet->daddr;
	cs.sport = packet->dport;
	cs.daddr = packet->saddr;
	cs.dport = packet->sport;

	f = find(cs);
	if(f != NULL){
		f->add_packet(packet, 0);
	}else{
		cs.saddr = packet->saddr;
		cs.daddr = packet->daddr;
		cs.sport = packet->sport;
		cs.dport = packet->dport;

		f = find(cs);
		if(f != NULL){
			f->add_packet(packet, 1);
		}else{
			/*
			 * New flow record.
			 */
			if(packet->tcp_flags == TcpHeader::TH_SYN){
				f = add(cs);
				f->add_packet(packet, 1);
			}else{
				delete packet;
			}
		}
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int FlowHashTable::clear()
{
	for(int i=0; i<size; i++){
		pthread_mutex_lock(&(flowHashTable[i]->mutex));
	}

	Flow	*f = NULL;
	flow_cnt = 0;
	for(int i=0; i<size; i++){
		while(flowHashTable[i]->first != NULL ){
			f = flowHashTable[i]->first;
			flowHashTable[i]->first = flowHashTable[i]->first->next;
			delete f;
			flowHashTable[i]->elm_cnt--;
		}
		flowHashTable[i]->first = NULL;
		flowHashTable[i]->last = NULL;
	}

	for(int i=0; i<size; i++){
		pthread_mutex_unlock(&(flowHashTable[i]->mutex));
	}
	return 0;
}

/* Initiate the flow hash table with no flows */
void FlowHashTable::reset()
{
	for(int i=0; i<size; i++){
		free(flowHashTable[i]);
	}
}

/* Return the size of hash table */
int FlowHashTable::getSize()
{
	return size;
}

/* Return the flow count of hash table */
int FlowHashTable::getFlowCnt()
{
	return flow_cnt;
}

/* Return the count of hash slots consumed */
int FlowHashTable::getSlotCnt()
{
	int i = 0;
	int slot_consumed = 0;
	
	for(i=0; i<size; i++)
	{
		pthread_mutex_lock(&(flowHashTable[i]->mutex));
	}
	
	for(i=0; i<size; i++)
	{
		if(flowHashTable[i]->first != NULL)
		{
			slot_consumed++;
		}
	}
	
	for(i=0; i<size; i++)
	{
		pthread_mutex_unlock(&(flowHashTable[i]->mutex));
	}
	return slot_consumed;
}

/*
 * Close a flow forcedly if the delta time is greater than timeout.
 * Then add the flow to flow queue.
 * Return the number of flows deleted forcedly.
 */
int FlowHashTable::flow_scrubber(const int timeout)
{
	unsigned long delta = 0;
	Flow *flow = NULL, *flow_next = NULL;
	struct timeval tv;
	struct timezone tz;
	int num = 0;

	for (int i=0; i<size; i++){
		pthread_mutex_lock(&(flowHashTable[i]->mutex));

		flow = flowHashTable[i]->first;
		while(flow != NULL){
			flow_next = flow->next;

			gettimeofday(&tv, &tz);
			delta = abs(tv.tv_sec - flow->last_action_sec);

			if (delta > timeout){
                printf("timeout");
				num++;
				flow->close = Flow::FORCED_CLOSE;	// Close flow forcedly.
				fq->enq(remove(flow));
			}

			flow = flow_next;
		}

		assert(flow == NULL);
		pthread_mutex_unlock(&(flowHashTable[i]->mutex));
	}
	return num;
}

/* Print hash table details for debugging */
void FlowHashTable::print(void)
{
	printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", getSize(), getSlotCnt(), getFlowCnt() );
}