/*
 * hash_table.c
 *
 *  Created on: Mar 16, 2012
 *      Author: front
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.h"
#include "data.h"
#include "packet.h"

#define HASH_FLOW(flow_socket) ( \
( (flow_socket.sport & 0xff) | ((flow_socket.dport & 0xff) << 8) | \
  ((flow_socket.saddr & 0xff) << 16) | ((flow_socket.daddr & 0xff) << 24) \
) % HASH_SIZE)


/* Initiate the flow hash table with no flows */
int 
flow_hash_init(void* d)
{
	int ret, i;
	Data* data = (Data*)d;

	data->flow_cnt = 0;
	for(i=0; i<HASH_SIZE; i++){
		data->flow_hash_table[i] = MALLOC(hash_mb_t, 1);
		data->flow_hash_table[i]->first = NULL;
		data->flow_hash_table[i]->last = NULL;
		data->flow_hash_table[i]->elm_cnt = 0;
		ret = pthread_mutex_init(&(data->flow_hash_table[i]->mutex), NULL);
		if (ret != 0)
			return -1;
	}
	return 0;
}

/* Create a new record in hash table according to flow's socket */
flow_t*
flow_hash_new(flow_s s)
{
	Data* data = {0};
	get_data(&data);
	flow_t *f = NULL;
	hash_mb_t *hm = NULL;

	f = flow_new();

	f->socket.saddr = s.saddr;
	f->socket.daddr = s.daddr;
	f->socket.sport = s.sport;
	f->socket.dport = s.dport;

	hm = data->flow_hash_table[HASH_FLOW(s)];
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
	data->flow_cnt++;

	pthread_mutex_unlock(&(hm->mutex));
	return f;
}

/* Delete a flow record in hash table */
flow_t*
flow_hash_delete(flow_t *f)
{
	Data* data = {0};
	get_data(&data);
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
	data->flow_cnt--;
	return f;
}

/* Try to find a flow record in hash table based on its socket */
flow_t*
flow_hash_find(flow_s s)
{
	Data* data = {0};
	get_data(&data);
	hash_mb_t *hm = NULL;
	flow_t	*flow = NULL;
	hm = data->flow_hash_table[HASH_FLOW(s)];
	pthread_mutex_lock(&(hm->mutex));

	if (hm->elm_cnt > 0){
		flow = hm->first;
		while(flow != NULL){
			if(flow_socket_cmp(&s, &flow->socket) == 0){
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
int 
flow_hash_add_packet(packet_t *packet)
{
	flow_s cs;
	flow_t *f = NULL;

	cs.saddr = packet->daddr;
	cs.sport = packet->dport;
	cs.daddr = packet->saddr;
	cs.dport = packet->sport;

	f = flow_hash_find(cs);
	if(f != NULL){
		flow_add_packet(f, packet, 0);
	}else{
		cs.saddr = packet->saddr;
		cs.daddr = packet->daddr;
		cs.sport = packet->sport;
		cs.dport = packet->dport;

		f = flow_hash_find(cs);
		if(f != NULL){
			flow_add_packet(f, packet, 1);
		}else{
			/*
			 * New flow record.
			 */
			if(packet->tcp_flags == TH_SYN){
				f = flow_hash_new(cs);
				flow_add_packet(f, packet, 1);
			}else{
				packet_free(packet);
			}
		}
	}
	return 0;
}

/* Clear the flow hash table thoroughly */
int 
flow_hash_clear(void)
{
	Data* data = {0};
	get_data(&data);
	int i = 0;
	for(i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(data->flow_hash_table[i]->mutex));
	}

	flow_t	*f = NULL;
	data->flow_cnt = 0;
	for(i=0; i<HASH_SIZE; i++){
		while(data->flow_hash_table[i]->first != NULL ){
			f = data->flow_hash_table[i]->first;
			data->flow_hash_table[i]->first = data->flow_hash_table[i]->first->next;
			flow_free(f);
			data->flow_hash_table[i]->elm_cnt--;
		}
		data->flow_hash_table[i]->first = NULL;
		data->flow_hash_table[i]->last = NULL;
	}

	for(i=0; i<HASH_SIZE; i++){
		pthread_mutex_unlock(&(data->flow_hash_table[i]->mutex));
	}
	return 0;
}

/* Return the size of hash table */
int
flow_hash_size(void)
{
	return HASH_SIZE;
}

/* Return the flow count of hash table */
int
flow_hash_fcnt(void)
{
	Data* data = {0};
	get_data(&data);
	return data->flow_cnt;
}

/* Return the count of hash slots consumed */
int
flow_hash_scnt(void)
{
	Data* data = {0};
	get_data(&data);
	int i = 0;
	int slot_consumed = 0;
	
	for(i=0; i<HASH_SIZE; i++)
	{
		pthread_mutex_lock(&(data->flow_hash_table[i]->mutex));
	}
	
	for(i=0; i<HASH_SIZE; i++)
	{
		if(data->flow_hash_table[i]->first != NULL)
		{
			slot_consumed++;
		}
	}
	
	for(i=0; i<HASH_SIZE; i++)
	{
		pthread_mutex_unlock(&(data->flow_hash_table[i]->mutex));
	}
	return slot_consumed;
}

/*
 * Close a flow forcedly if the delta time is lower than timeout.
 * Then add the flow to flow queue.
 * Return the number of flows deleted forcedly.
 */
int 
flow_scrubber(const int timeout)
{
	Data* data = {0};
	get_data(&data);
	int i = 0;
	unsigned long delta = 0;
	flow_t *flow = NULL, *flow_next = NULL;
	struct timeval tv;
	struct timezone tz;
	int num = 0;

	for (i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(data->flow_hash_table[i]->mutex));

		flow = data->flow_hash_table[i]->first;
		while(flow != NULL){
			flow_next = flow->next;

			gettimeofday(&tv, &tz);
			delta = abs(tv.tv_sec - flow->last_action_sec);

			if (delta > timeout){
                printf("timeout");
				num++;
				flow->close = FORCED_CLOSE;	// Close flow forcedly.
				flow_queue_enq(flow_hash_delete(flow));
			}

			flow = flow_next;
		}

		assert(flow == NULL);
		pthread_mutex_unlock(&(data->flow_hash_table[i]->mutex));
	}
	return num;
}

/* Print hash table details for debugging */
void 
flow_hash_print(void)
{
	printf("(Hash size)%d : (Consumed)%d : (Flows)%d\n", flow_hash_size(), flow_hash_scnt(), flow_hash_fcnt() );
}
