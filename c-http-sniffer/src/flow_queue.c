/*
 * queue.c
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

/************************
 * flow queue functions *
 ************************/
int 
flow_queue_init(void* d)
{
	Data* data = (Data*)d;
	int ret = 0;
	ret = pthread_mutex_init(&(data->mutex_queue), NULL);
	if(ret != 0){
		return -1;
	}

	data->flow_queue_first = NULL;
	data->flow_queue_last = NULL;
	data->flow_qlen = 0;

	return 0;
}

int 
flow_queue_enq(flow_t *flow)
{
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&(data->mutex_queue));


	if (data->flow_qlen == 0){
		data->flow_queue_first = flow;
		data->flow_queue_last = flow;
		data->flow_queue_last->next = NULL;
		data->flow_qlen++;
		pthread_mutex_unlock(&(data->mutex_queue));
		return 0;
	}

	data->flow_queue_last->next = flow;
	data->flow_queue_last = flow;
	data->flow_queue_last->next = NULL;
	data->flow_qlen++;

	pthread_mutex_unlock(&data->mutex_queue);
	return 0;
}

flow_t*
flow_queue_deq(void)
{
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&(data->mutex_queue));
	flow_t *f = NULL;
	if(data->flow_qlen == 0){
		pthread_mutex_unlock(&(data->mutex_queue));
		return NULL;
	}else if(data->flow_qlen == 1){
		data->flow_queue_last = NULL;
	}

	f = data->flow_queue_first;

	if(data->flow_queue_first != NULL) {
		data->flow_queue_first = data->flow_queue_first->next;
		data->flow_qlen--;
	} else {
		printf("Flow queue first was null");
	}

	pthread_mutex_unlock(&data->mutex_queue);
	return f;
}

int 
flow_queue_clear(void)
{
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&(data->mutex_queue));

	flow_t *f;

	while(data->flow_qlen > 0){
		f = data->flow_queue_first;
		data->flow_queue_first = data->flow_queue_first->next;
		flow_free(f);
		data->flow_qlen--;
	}
	data->flow_queue_first =  NULL;
	data->flow_queue_last = NULL;
	data->flow_qlen = 0;

	pthread_mutex_unlock(&(data->mutex_queue));
	return 0;
}

int 
flow_queue_len(void)
{
	Data* data = {0};
	get_data(&data);
	return data->flow_qlen;
}

void 
flow_queue_print(void)
{
	printf("(Flow queue length)%d\n", flow_queue_len());
}
