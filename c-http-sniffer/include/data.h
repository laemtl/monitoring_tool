#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include "flow.h"
#include "hash_table.h"
#include "queue.h"
#include "client.h"

#ifndef DEBUGGING
#define DEBUGGING 2 
#endif

#define EPSILON	1e-7
#define CLOSE(a,b) (fabs(a - b)<EPSILON)

typedef struct _int_metric int_metric;
struct _int_metric {
    int value;
	pthread_mutex_t mutex;
};

typedef struct _dbl_metric dbl_metric;
struct _dbl_metric {
    double value;
    pthread_mutex_t mutex;
};

typedef struct _metric Metric;
struct _metric {
    BOOL active;
    int_metric subtotal;
    int_metric total;
    dbl_metric sum;
    dbl_metric min;
    dbl_metric max;
};

// memset to 0 on init
typedef struct _data Data;
struct _data {
    BOOL server_mode;
    int client_sock;

    pthread_mutex_t lock;
    
    // 0: started, 1: running, -1: stopped
    int status;
    const char* interface;
    uint32_t interval;
    uint32_t int_step;
    uint32_t duration;
    //uint32_t topclient_cnt;

    BOOL has_client_ip;
    BOOL has_client_port;
    Addr client;

    uint32_t stamp;
    
    Addr destination;
    int req_tot;
    int rsp_tot;
    int rsp_int_tot;
    int rsp_sec_tot;
    int flow_tot;
    
    Metric rst;
    Metric err_rate;
    Metric req_rate;
    
    Metric tp;
    Addr server;

    hash_t conn_ht;
    Metric conn_rate;
    
    Queue raw_pkt_queue;

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
    pthread_mutex_t pkt_queue_mutex;
 
    // flow_hash_table.c
    hash_mb_t *flow_hash_table[HASH_SIZE];
    int flow_cnt;	/* flows live in hash table */

    hash_t client_ht;    
    BOOL client_active;
    
    hash_t req_path_ht;
    BOOL req_path_active;
    BOOL req_method_active;
    
    // TODO: lock
    int req_type[41];
    BOOL req_type_active;
    
    int rsp_status[599];
    BOOL rsp_status_active;
};

typedef struct _capt Capture;
struct _capt {
    int fd;
	//void (*pkt_handler)(void*); 
	int livemode;
    Data* data;
};

void destr_fn(void *param);
void init_once_metric(Metric* metric);
void reset_metric(Metric* metric);
void reset(Data* data);
Data* init_data();
void delete_data();
static void thread_key_setup();
void thread_init(Data* d);
void print_tid();
void get_data(Data** d);
double get_metric_min(Metric metric);
double get_rst_avg();
double get_err_rate();
double get_err_rate_subtotals();
double get_req_rate();
void add_metric_sum(Metric* metric, double amt);
void inc_metric_total(Metric* metric);
void inc_metric_subtotal(Metric* metric);
void reset_metric_subtotal(Metric* metric);
void update_metric_min(Metric* metric, double value);
void update_metric_max(Metric* metric, double value);
BOOL is_server_mode();
void extract_data(flow_t *flow);
Result* get_result();
void process_rate(Data* data);
void process_data(int tid);
void print_data(Result* result);

#endif
