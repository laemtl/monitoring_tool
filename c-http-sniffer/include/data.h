#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "flow.h"

#define HASH_SIZE	13200

#ifndef DEBUGGING
#define DEBUGGING 2 
#endif

#define EPSILON	1e-7
#define CLOSE(a,b) (fabs(a - b)<EPSILON)

typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
//    int		    elm_cnt;
};

typedef struct _path Path;
struct _path {
	char*       path;
    int		    elm_cnt;
};

typedef struct _metric_el Metric_el;
struct _metric_el {
    double value;
	pthread_mutex_t mutex;
};

typedef struct _metric Metric;
struct _metric {
    int status;
    Metric_el subtotal;
    Metric_el total;
    Metric_el min;
    Metric_el max;
};

// memset to 0 on init
typedef struct _data Data;
struct _data {
    bool server_mode;
    int client_sock;
    
    // 0: started, 1: running, -1: stopped
    int status;
    const char* interface;
    uint32_t interval;
    uint32_t int_step;
    uint32_t duration;

    Addr destination;
    
    Metric rst;
    Metric err_rate;
    Metric req_rate;
    Metric tp;
    
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
 
    // flow_hash_table.c
    hash_mb_t *flow_hash_table[HASH_SIZE];
    int flow_cnt;	/* flows live in hash table */

    Addr *addr_hash_table[HASH_SIZE];
    int addr_cnt;
    
    Path *path_hash_table[HASH_SIZE];
    int path_cnt;
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
