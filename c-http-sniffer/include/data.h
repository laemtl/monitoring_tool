#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include "flow.h"
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#define HASH_SIZE	13200

#ifndef DEBUGGING
#define DEBUGGING 2 
#endif

// memset to 0 on init
typedef struct _data Data;
struct _data {
    bool running;
    const char* interface;
    uint32_t interval;
    uint32_t duration;

    uint32_t req_t;
    uint32_t err_t;
    double rst_t;
    
    double rst_min;
    double rst_max;

    double err_rate_min;
    double err_rate_max;

    double req_rate_min;
    double req_rate_max;

    bool server_mode;
    int client_sock;

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
    const char* interface;
    int client_sock;

	double rst_avg;
    double rst_min;
    double rst_max;

	double err_rate;
    double err_rate_min;
    double err_rate_max;

    double req_rate;
    double req_rate_min;
    double req_rate_max;
    
    double tp;
};

typedef struct _capt Capture;
struct _capt {
    int fd;
	void (*pkt_handler)(void*); 
	int livemode;
    Data* data;
};

void extract_data(const flow_t *flow);
void process_data ();
void print_data();
void thread_init(Data* d);

#endif
