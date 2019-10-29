#ifndef __DATA_H__
#define __DATA_H__

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include "flow.h"
#include "hash_table.h"
#include "queue.h"
#include "result.h"

#include "metricManager.hpp"
#include "eventManager.hpp"

#ifndef DEBUGGING
#define DEBUGGING 0 
#endif

#define EPSILON	1e-7
#define CLOSE(a,b) (fabs(a - b)<EPSILON)

class Analysis {
    public:
        Analysis();
        void activeMetrics(vector<bool> status);

        EventManager* eventManager;
        MetricManager* metricManager;
};






/*class Analysis {
    private:

    public:
        int flow_req;
        int flow_rsp;
        
        int pak;
        int pak_deq;
        
        int rsp_n;
        int req_n;

        int raw_req;
        int raw_rsp;

        EventManager* eventManager;

        bool debug;
        bool serverMode;
        int clientSock;

        //uint32_t stamp;

        // 0: started, 1: running, -1: stopped
        int status;
        const char* interface;
        char* ipaddress;
        uint32_t interval;
        uint32_t duration;

        int fd; 
	    int livemode;
    
        bool has_client_ip;
        bool has_client_port;
        sockaddr_in client;

        bool has_server_ip;
        bool has_server_port;
        sockaddr_in server;

        // Move this in timer
        uint32_t int_step;
        
        Queue<RawPacket>* rawPktQueue;
        PacketQueue* pktQueue;
        FlowHashTable* flowHashTable;
        FlowQueue* flowQueue;

        Analysis(bool debug, const char* interface, uint32_t interval, char* ipaddress);
        bool isServerMode();
        void start();
        void debugging_print();

        void processFlowHashTable();
        void processFlowQueue();
        void processPacketQueue();

        void printData();
        void processData(int sig);
        void printTid();
        void capture_main();
        void stop(int sig);
        void scrubbingFlowHashTable();
        Packet* packetPreprocess(char *raw_data, const struct pcap_pkthdr *pkthdr);
        void processRawPktQueue();

        void extract_data(Flow *flow);
};*/













typedef struct _addr Addr;
struct _addr {
	u_int32_t	ip;
	u_int16_t	port;
};

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

    BOOL has_server_ip;
    BOOL has_server_port;
    Addr server;

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
    
    u_int32_t sec_ref_value;
    u_int32_t sec_rev_ref_value;

    u_int32_t int_ref_value;
    u_int32_t int_rev_ref_value;
    
    Metric tp;
    Metric tp_rev;

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

    Analysis* analysis;
};

typedef struct _capt Capture;
struct _capt {
    int fd; 
	int livemode;
    Data* data;
};

void destr_fn(void *param);
void init_once_metric(Metric* metric);
void reset_metric(Metric* metric);
void reset(Data* data);
Data* init_data();
void delete_data(Data* data);
void thread_key_setup();
void thread_init(Data* d);
void print_tid();
void get_data(Data** d);
double get_metric_min(Metric metric);
double get_rst_avg();
double get_err_rate();
double get_err_rate_subtotal();
double get_req_rate();
double get_tp_avg();
double get_tp_rev_avg();
double get_conn_rate();
void add_metric_sum(Metric* metric, double amt);
void add_metric_total(Metric* metric, int amt);
void add_metric_subtotal(Metric* metric, int amt);
void reset_metric_subtotal(Metric* metric);
void update_metric_min(Metric* metric, double value);
void update_metric_max(Metric* metric, double value);
BOOL is_server_mode();
void extract_data(flow_t *flow);
void compute_tp(u_int32_t payload, Data* data);
void compute_tp_rev(u_int32_t payload, Data* data);
void compute_req_rate(Data* data);
void compute_rst(http_pair_t *h, Data* data);
void compute_err_rate(int status, Data* data);
void extract_freq_ht(hash_t* ht, Result* r, void (*cfl_add_fn)(void*, int, Result*));
void extract_freq_ar(int* ar, int size, Result* r, void (*cfl_add_fn)(int, int, Result*));
Result* get_result();
void process_rate(Data* data);
void flow_hash_process();
void free_result(Result* result);
void process_data(int sig);
void print_data(Result* result);

#endif
