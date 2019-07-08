#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "flow.h"
#include "hash_table.h"
#include "client.h"

#ifndef DEBUGGING
#define DEBUGGING 2 
#endif


#define EPSILON	1e-7
#define CLOSE(a,b) (fabs(a - b)<EPSILON)

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

typedef struct _top_list Top_list;
struct _top_list {
	void** top_list;
    pthread_mutex_t mutex;
    int size;
    int count;
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
    uint32_t i;
    
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

    hash_t client_ht;
    Top_list client_tl;
    
    hash_t path_ht;
    Top_list path_tl;
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

    int client_tot;
    int path_tot;
};

typedef struct _capt Capture;
struct _capt {
    int fd;
	void (*pkt_handler)(void*); 
	int livemode;
    Data* data;
};

void destr_fn(void *param);
void init_once_metric(Metric* metric);
void reset_metric(Metric* metric);
void reset(Data* data);
void init_data(Data* data);
static void thread_key_setup();
void thread_init(Data* d);
void print_tid();
void get_data(Data** d);
double get_metric_min(Metric metric);
double get_rst_avg(Metric metric);
double get_err_rate();
double get_err_rate_subtotals();
double get_req_rate();
void inc_metric_total(Metric* metric, double amt);
void inc_metric_subtotal(Metric* metric, double amt);
void reset_metric_subtotal(Metric* metric);
void update_metric_min(Metric* metric, double value);
void update_metric_max(Metric* metric, double value);
bool is_server_mode();
void extract_data(const flow_t *flow);
Result* get_result(Result* result);
void process_rate(Data* data);
void process_data();
void print_data(Result* result);

#endif
