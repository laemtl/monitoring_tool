#ifndef GLOBAL_H
#define GLOBAL_H

#include "flow.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define HASH_SIZE	13200

// memset to 0 on init

typedef struct _data Data;
struct _data {
    uint32_t req_t;
    uint32_t err_t;
    double rst_t;

    bool server_mode;
    int sfd;

    // data.c
    pthread_mutex_t lock;
    
    // flow_queue.c
    /* Queue vars */
    pthread_mutex_t mutex_queue;
    flow_t *flow_queue_first;
    flow_t *flow_queue_last;
    int flow_qlen;	/* flow queue length */
    
    // packet_queue.c
    /* Queue vars */
    unsigned int que_len;
    packet_t *pkt_first;
    packet_t *pkt_last;
    pthread_mutex_t mutex;
 
    // hash_table.c
    hash_mb_t *flow_hash_table[HASH_SIZE];
    int flow_cnt;	/* flows live in hash table */
};

typedef struct _result Result;
struct _result {
	double rst_avg;
	double err_rate;
};

#endif
