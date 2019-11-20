#include "flowHashTable.hpp"

/* Initiate the flow hash table with no flows */
FlowHashTable::FlowHashTable(Queue* fq) : fq(fq) {
	flow_cnt = 0;
	for(int i=0; i<HASH_SIZE; i++){
		flow_hash_table[i] = MALLOC(hash_mb_t, 1);
		flow_hash_table[i]->first = NULL;
		flow_hash_table[i]->last = NULL;
		flow_hash_table[i]->elm_cnt = 0;
		pthread_mutex_init(&(flow_hash_table[i]->mutex), NULL);
	}
}

/* Create a new record in hash table according to flow's socket */
Flow* FlowHashTable::create(flow_s s) {
	Flow* f = NULL;
	hash_mb_t *hm = NULL;

	f = new Flow(fq, this);

	f->socket.saddr = s.saddr;
	f->socket.daddr = s.daddr;
	f->socket.sport = s.sport;
	f->socket.dport = s.dport;

	hm = flow_hash_table[HASH_FLOW(s)];
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
Flow* FlowHashTable::remove(Flow* f) {
	hash_mb_t *hm = NULL;
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
Flow* FlowHashTable::find(flow_s s) {
	hash_mb_t *hm = NULL;
	Flow *flow = NULL;
	hm = flow_hash_table[HASH_FLOW(s)];
	pthread_mutex_lock(&(hm->mutex));

	if (hm->elm_cnt > 0){
		flow = hm->first;
		while(flow != NULL){
			if(flow->flow_socket_cmp(&s) == 0){
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
int FlowHashTable::add_packet(packet_t *packet) {
	flow_s cs;
	Flow* f = NULL;

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
			// check if we have the first packet of either a request or response
			if(packet->http == HTTP_REQ) {
				f = create(cs);
				f->add_packet(packet, 1);
			} else if(packet->http == HTTP_RSP) {
				// Invert source and destination
				cs.saddr = packet->daddr;
				cs.sport = packet->dport;
				cs.daddr = packet->saddr;
				cs.dport = packet->sport;
				
				f = create(cs);
				f->add_packet(packet, 0);
			} else {
				packet_free(packet);
			}


			/*
			* New flow record.
			*/
			/*if(packet->tcp_flags == TH_SYN){
				f = create(cs);
				flow_add_packet(f, packet, 1);
			}else{
				packet_free(packet);
			}*/
		}
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int FlowHashTable::clear() {
	for(int i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(flow_hash_table[i]->mutex));
	}

	Flow *f = NULL;
	flow_cnt = 0;
	for(int i=0; i<HASH_SIZE; i++){
		while(flow_hash_table[i]->first != NULL ){
			f = flow_hash_table[i]->first;
			flow_hash_table[i]->first = flow_hash_table[i]->first->next;
			delete f;
			flow_hash_table[i]->elm_cnt--;
		}
		flow_hash_table[i]->first = NULL;
		flow_hash_table[i]->last = NULL;
	}

	for(int i=0; i<HASH_SIZE; i++){
		pthread_mutex_unlock(&(flow_hash_table[i]->mutex));
	}
	return 0;
}

/* Initiate the flow hash table with no flows */
void FlowHashTable::reset() {
	for(int i=0; i<HASH_SIZE; i++){
		free(flow_hash_table[i]);
	}
}

/* Return the size of hash table */
int FlowHashTable::size() {
	return HASH_SIZE;
}

/* Return the flow count of hash table */
int FlowHashTable::fcnt() {
	return flow_cnt;
}

/* Return the count of hash slots consumed */
int FlowHashTable::scnt() {
	int slot_consumed = 0;
	for(int i=0; i<HASH_SIZE; i++)
	{
		pthread_mutex_lock(&(flow_hash_table[i]->mutex));
	}
	
	for(int i=0; i<HASH_SIZE; i++)
	{
		if(flow_hash_table[i]->first != NULL)
		{
			slot_consumed++;
		}
	}
	
	for(int i=0; i<HASH_SIZE; i++)
	{
		pthread_mutex_unlock(&(flow_hash_table[i]->mutex));
	}
	return slot_consumed;
}

/*
* Close a flow forcedly if the delta time is greater than timeout.
* Then add the flow to flow queue.
* Return the number of flows deleted forcedly.
*/
int FlowHashTable::flow_scrubber(const int timeout){
	int i = 0;
	unsigned long delta = 0;
	Flow* flow = NULL, *flow_next = NULL;
	struct timeval tv;
	struct timezone tz;
	int num = 0;

	for (i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(flow_hash_table[i]->mutex));

		flow = flow_hash_table[i]->first;
		while(flow != NULL){
			flow_next = flow->next;

			gettimeofday(&tv, &tz);
			delta = abs(tv.tv_sec - flow->last_action_sec);

			if (delta > timeout){
				printf("timeout");
				num++;
				flow->close = FORCED_CLOSE;	// Close flow forcedly.
				fq->enq(remove(flow));
			}

			flow = flow_next;
		}

		assert(flow == NULL);
		pthread_mutex_unlock(&(flow_hash_table[i]->mutex));
	}
	return num;
}

/* Print hash table details for debugging */
void FlowHashTable::print() {
	printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", size(), scnt(), fcnt() );
}