/*
 * queue.c
 *
 *  Created on: Mar 16, 2012
 *      Author: chenxm
 *		Email: chen_xm@sjtu.edu.cn
 */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "packet.h"
#include "util.h"
#include "analysis.hpp"


BOOL 
packet_queue_init(void* d)
{
	int ret;
	Data* data = (Data*)d;
	
	data->pkt_first = NULL;
	data->pkt_last = NULL;
	data->que_len = 0;
	ret = pthread_mutex_init(&data->pkt_queue_mutex, NULL);
	if(ret != 0)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL 
packet_queue_enq(packet_t *pkt)
{
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&data->pkt_queue_mutex);
	if(data->que_len == 0)
	{
		data->pkt_first = pkt;
		data->pkt_last = pkt;
		data->pkt_last->next = NULL;
		data->que_len++;
		pthread_mutex_unlock(&data->pkt_queue_mutex);
		return TRUE;
	}
	data->pkt_last->next = pkt;
	data->pkt_last = pkt;
	data->pkt_last->next = NULL;
	data->que_len++;
	pthread_mutex_unlock(&data->pkt_queue_mutex);
	return TRUE;
}

packet_t *
packet_queue_deq(void)
{
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&data->pkt_queue_mutex);
	packet_t *pkt;
	if(data->que_len == 0)
	{
		pthread_mutex_unlock(&data->pkt_queue_mutex);
		return NULL;
	}
	else if (data->que_len == 1 )
	{
		data->pkt_last = NULL;
	}
	pkt = data->pkt_first;
	data->pkt_first = data->pkt_first->next;
	data->que_len--;
	pthread_mutex_unlock(&data->pkt_queue_mutex);
	return pkt;
}

BOOL 
packet_queue_clr(void)
{
Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&data->pkt_queue_mutex);
	packet_t *pkt;
	while(data->que_len > 0)
	{
		pkt = data->pkt_first;
		data->pkt_first = data->pkt_first->next;
		packet_free(pkt);
		data->que_len--;
	}
	data->pkt_first =  NULL;
	data->pkt_last = NULL;
	data->que_len = 0;
	pthread_mutex_unlock(&data->pkt_queue_mutex);
	return TRUE;
}

unsigned int 
packet_queue_len(void)
{
	Data* data = {0};
	get_data(&data);
	return data->que_len;
}

void 
packet_queue_print(void)
{
	printf("(Packet queue length)%d\n", packet_queue_len());
}
