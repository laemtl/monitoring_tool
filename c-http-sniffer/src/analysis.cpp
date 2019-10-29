#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/syscall.h>
#include <inttypes.h>
#include <float.h>
#include <string.h>
#include <unistd.h>
#include "analysis.hpp"
#include "req_path.h"
#include "conn.h"
#include "req_type.h"
#include "rsp_status.h"
#include "client.h"
#include "server.h"
#include "util.h"

extern int flow_req;
extern int flow_rsp;
extern int pak;
extern int pak_deq;
extern int rsp_n;
extern int req_n;

extern BOOL debug;

Analysis::Analysis() {
    eventManager = new EventManager();
    metricManager = new MetricManager();
};

void Analysis::activeMetrics(vector<bool> status) {

    if(status.size() != metricManager->metrics.size()) {
        cout << "Invalid status size";
        pthread_exit(NULL);
    }

    for (unsigned i = 0; i < status.size(); i++) {
        if(!status.at(i))  metricManager->metrics.erase( metricManager->metrics.begin() + i);
    }

    // Register
    for (unsigned i = 0; i <  metricManager->metrics.size(); i++) {
        metricManager->metrics.at(i)->subscribe(eventManager);
    }
}

//int d = 0;

/* Output flow's brief to stdout */
/** Added Functionality: Function now takes file descriptor as arguement
 *  file descriptor should point to sockt in remote node where
 *  logger is listening
**/

static pthread_key_t data;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

void destr_fn(void *param) {
}

void init_once_metric(Metric* metric) {
	pthread_mutex_init(&(metric->total.mutex), NULL);
	pthread_mutex_init(&(metric->subtotal.mutex), NULL);
	pthread_mutex_init(&(metric->sum.mutex), NULL);
	pthread_mutex_init(&(metric->min.mutex), NULL);
	pthread_mutex_init(&(metric->max.mutex), NULL);
}

void reset_metric(Metric* metric) {
	metric->total.value = 0;
	metric->subtotal.value = 0;
	metric->sum.value = 0;
	metric->min.value = DBL_MAX;
	metric->max.value = 0;
}

void reset(Data* data) {
	reset_metric(&(data->rst));
	reset_metric(&(data->err_rate));
	reset_metric(&(data->req_rate));
	reset_metric(&(data->tp));
	reset_metric(&(data->tp_rev));
	reset_metric(&(data->conn_rate));

	data->int_step = 0;

	// TODO: clean on exit
	//hash_clear(&(data->client_ht)); 
}

Data* init_data() {
	Data* data = CALLOC(Data, 1);
	
	pthread_mutex_init(&(data->lock), NULL);
	
	init_once_metric(&(data->rst));
	init_once_metric(&(data->err_rate));
	init_once_metric(&(data->req_rate));
	init_once_metric(&(data->tp));
	init_once_metric(&(data->tp_rev));

	reset(data);	
	data->status = 0;

	hash_init(&(data->conn_ht), conn_hash_fn, addr_compare, update_attr, delete_attr);
	//tl_init(&(data->conn_ht.tl));

	hash_init(&(data->client_ht), client_hash_fn, ip_compare, update_attr, delete_attr);
	hash_init(&(data->req_path_ht), req_path_hash_fn, req_path_compare, update_attr, delete_attr);

	return data;
};

void delete_data(Data* data) {
	hash_clear(&(data->client_ht));
	hash_reset(&(data->client_ht));
	
	hash_clear(&(data->conn_ht));
	hash_reset(&(data->conn_ht));
	
	hash_clear(&(data->req_path_ht));
	hash_reset(&(data->req_path_ht));
	
	free(data);
}

void thread_key_setup() {
	int status;
    if ((status = pthread_key_create(&data, destr_fn )) < 0) {
        error("pthread_key_create failed");
    }
}

void thread_init(Data* d) {
    pthread_once(&init_done, thread_key_setup);

	if (pthread_setspecific(data, d) <  0) {
		error("pthread_setspecific failed\n");
	}
}

void print_tid() {
	pid_t x = syscall(__NR_gettid);
	printf("pid: %ld \n", (long)x);
}

void get_data(Data** d) {	
	*d = (Data *)pthread_getspecific(data);
    if(d == NULL) {
        error("pthread_getspecific failed\n");
	}
}

double get_metric_min(Metric metric) {
	if(CLOSE(metric.min.value, DBL_MAX)) {
	    return 0;
    }
	return metric.min.value;
}

double get_rst_avg() {
	Data* data = {0};
	get_data(&data);

	int req_total = data->rst.total.value; 
	if (req_total != 0) return data->rst.sum.value/req_total;
	return 0;
}

double get_err_rate() {
	Data* data = {0};
	get_data(&data);

	double result = 0;
	if (data->rsp_int_tot > 0) {
		result = (double)data->err_rate.total.value/data->rsp_int_tot;
	}
	return result;
}

// Get error rate with subtotal
double get_err_rate_subtotal() {
	Data* data = {0};
	get_data(&data);

	double result = 0;
	if (data->rsp_sec_tot > 0) {
		result = (double)data->err_rate.subtotal.value/data->rsp_sec_tot;
	}
	return result;
}

double get_req_rate() {
	Data* data = {0};
	get_data(&data);

	int req_total = data->req_rate.total.value;
	double result = 0;
	if (data->interval > 0) {
		result = (double)req_total/data->interval;
	}
	return result;
}

double get_tp_avg() {
	Data* data = {0};
	get_data(&data);

	if(data->tp.total.value <= 0) return 0;

	double result = (double)(data->tp.total.value - data->int_ref_value)/data->interval;
	data->int_ref_value = data->tp.total.value;
	return result;
}

double get_tp_rev_avg() {
	Data* data = {0};
	get_data(&data);

	if(data->tp_rev.total.value <= 0) return 0;

	double result = (double)(data->tp_rev.total.value - data->int_rev_ref_value)/data->interval;
	data->int_rev_ref_value = data->tp_rev.total.value;

	return result;
}

double get_conn_rate() {
	Data* data = {0};
	get_data(&data);

	return (double)data->conn_rate.sum.value/data->interval;
}

void add_metric_sum(Metric* metric, double amt) {
    pthread_mutex_lock(&(metric->sum.mutex));
	metric->sum.value += amt;
	pthread_mutex_unlock(&(metric->sum.mutex));
}

void add_metric_total(Metric* metric, int amt) {
    pthread_mutex_lock(&(metric->total.mutex));
	metric->total.value += amt;
	pthread_mutex_unlock(&(metric->total.mutex));
}

void add_metric_subtotal(Metric* metric, int amt) {
	pthread_mutex_lock(&(metric->subtotal.mutex));
	metric->subtotal.value += amt;
	pthread_mutex_unlock(&(metric->subtotal.mutex)); 
}

void reset_metric_subtotal(Metric* metric) {
	pthread_mutex_lock(&(metric->subtotal.mutex));
	metric->subtotal.value = 0;
	pthread_mutex_unlock(&(metric->subtotal.mutex)); 
}

void update_metric_min(Metric* metric, double value) {
	pthread_mutex_lock(&(metric->min.mutex));
	if(value < metric->min.value) metric->min.value = value;
	pthread_mutex_unlock(&(metric->min.mutex)); 
}

void update_metric_max(Metric* metric, double value) {
	pthread_mutex_lock(&(metric->max.mutex));
	if(value > metric->max.value) metric->max.value = value;
	pthread_mutex_unlock(&(metric->max.mutex)); 
}

BOOL is_server_mode() {
    Data* data = {0};
	get_data(&data);
	return data->server_mode;
}

void extract_data(flow_t *flow){	
	Data* data = {0};
	get_data(&data);
	pthread_mutex_lock(&(data->lock));

	if(data->has_client_ip) {
		if(data->client.ip != flow->socket.saddr) {
			pthread_mutex_unlock(&(data->lock));
			return;
		}
		
		if(data->has_client_port) {
			if(data->client.port != flow->socket.sport) {
				pthread_mutex_unlock(&(data->lock));
				return;
			}
		}

		// Throughput
		if(data->has_server_ip) {
			if(data->server.ip != flow->socket.daddr) {
				pthread_mutex_unlock(&(data->lock));
				return;
			}
			
			if(data->has_server_port) {
				if(data->server.port != flow->socket.dport) {
					pthread_mutex_unlock(&(data->lock));
					return;
				}
			}
		}

		compute_tp(flow->payload_src, data);
		compute_tp_rev(flow->payload_dst, data);
	}

	if(!flow->processed) {
		add_conn(flow->socket.saddr, flow->socket.sport, data);
		data->flow_tot++;
		flow->processed = TRUE;		

		data->analysis->eventManager->newFlowReceived->notify(flow);		
	}
	
	// Need client/server value
	data->analysis->eventManager->flowUpdate->notify(flow);

	if (flow->http_f != NULL){        	
		http_pair_t *pair = flow->http_f;
		while(pair != NULL) {
			if(pair->request_header != NULL) {
				if(!pair->req_processed) {
					pair->req_processed = TRUE;
					request_t *req = pair->request_header;
					flow_req++;

					data->analysis->eventManager->requestReceived->notify(pair, flow);

					data->req_tot++;
					add_client(flow->socket.saddr, data);
					add_req_path(req->uri, data);
					add_req_type(req->method, data);
					compute_req_rate(data);
				}
				
				if(pair->rsp_processed) {
					http_pair_t* next = pair->next;

					if(next != NULL) {
						flow->http_f = next;
						http_free(pair);
					}
					pair = next;
					continue;
				}

				if(pair->response_header != NULL) {
					pair->rsp_processed = TRUE;
					flow_rsp++;
					data->rsp_tot++;
					data->rsp_int_tot++;
    				data->rsp_sec_tot++;

					data->analysis->eventManager->responseReceived->notify(pair, flow);

					compute_rst(pair, data);
					response_t *rsp = pair->response_header;					
					compute_err_rate(rsp->status, data);
					add_rsp_status(rsp->status, data);
				}
				//if(h->response_header == NULL) d++;
			}	   
			pair = pair->next;
		}
	}
	pthread_mutex_unlock(&(data->lock));
}

void compute_tp(u_int32_t payload, Data* data) {
	if(!data->tp.active) return;

	data->tp.total.value = payload;
}

void compute_tp_rev(u_int32_t payload, Data* data) {
	if(!data->tp_rev.active) return;

	data->tp_rev.total.value = payload;
}

void compute_req_rate(Data* data) {
	if(!data->req_rate.active) return;

	add_metric_subtotal(&(data->req_rate), 1);
}

void compute_rst(http_pair_t *h, Data* data) {
	if(!data->rst.active) return;

	// Compute response time
	double rst = (h->rsp_fb_sec + h->rsp_fb_usec * 0.000001) - (h->req_fb_sec + h->req_fb_usec * 0.000001);

	response_t *rsp = h->response_header;
	request_t *req = h->request_header;

	if(rst < 0 ) {
		printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
		printf("aknowledgment: %ld \n", rsp->acknowledgement);
	}
	
	add_metric_total(&(data->rst), 1);
	add_metric_sum(&(data->rst), rst);
	update_metric_min(&(data->rst), rst);
	update_metric_max(&(data->rst), rst);
}

void compute_err_rate(int status, Data* data) {
	if(!data->err_rate.active) return;

	// Extract first digit of status code
	int i = status;
	while (i>=10) i=i/10;  
	if (i>3) {
		add_metric_subtotal(&(data->err_rate), 1);
	}
}

void extract_freq_ht(hash_t* ht, Result* r, void (*cfl_add_fn)(void*, int, Result*)) {
	int i;
	//hash_mb_t *hm = NULL;
	//node	*e = NULL;

	for(i=0; i<ht->size; i++){
		pthread_mutex_lock(&(ht->buckets[i]->mutex));
	}

	for(i=0; i<ht->size; i++){
		node* n = ht->buckets[i]->first;
		while(n != NULL) {
			Attr* a = (Attr*) n->value;
			cfl_add_fn(a->elem, a->cnt, r);
			n = n->next;
		}
	}

	for(i=0; i<ht->size; i++){
		pthread_mutex_unlock(&(ht->buckets[i]->mutex));
	}
}

void extract_freq_ar(int* ar, int size, Result* r, void (*cfl_add_fn)(int, int, Result*)) {
	Data* data = {0};
	get_data(&data);

	int i;
	for(i=0; i<size; i++){
		cfl_add_fn(i, ar[i], r);
	}
}

// TODO: check if status active
Result* get_result() {
    Data* data = {0};
	get_data(&data);

	Result* result = CALLOC(Result, 1);

	// TODO: We need a lock
	result->interface = (char*)data->interface;	
	result->client_sock = data->client_sock;

	if(data->rst.active) {
		result->rst_avg = get_rst_avg();
		result->rst_min = get_metric_min(data->rst);
    	result->rst_max = data->rst.max.value;
	}
	
	if(data->err_rate.active) {
		result->err_rate = get_err_rate();    
		result->err_rate_min = get_metric_min(data->err_rate);   
		result->err_rate_max = data->err_rate.max.value;   
	}

	if(data->req_rate.active) {
		result->req_rate = get_req_rate(); 
		result->req_rate_min = get_metric_min(data->req_rate);
		result->req_rate_max = data->req_rate.max.value;
	}

	if(data->tp.active) {
		result->tp_avg = get_tp_avg();
		result->tp_min = get_metric_min(data->tp);
		result->tp_max = data->tp.max.value;
	}

	if(data->tp_rev.active) {
		result->tp_rev_avg = get_tp_rev_avg();
		result->tp_rev_min = get_metric_min(data->tp_rev);
		result->tp_rev_max = data->tp_rev.max.value;
	}

	if(data->conn_rate.active) {
		result->conn_rate = get_conn_rate();
		result->conn_rate_min = data->conn_rate.min.value;
		result->conn_rate_max = data->conn_rate.max.value;
	
		//result->conn_tl = data->conn_ht.tl;
	}

	if(data->client_active) {
		cfl_init(&(result->client));
		extract_freq_ht(&(data->client_ht), result, client_cfl_add);
	}

	// TODO: can have duplicate path (ex: path/meth1 & path/meth2)
	if(data->req_path_active || data->req_method_active) {
		cfl_init(&(result->req_path));
		cfl_init(&(result->req_method));
		extract_freq_ht(&(data->req_path_ht), result, req_path_cfl_add);
	}

	if(data->req_type_active) {
		cfl_init(&(result->req_type));
		int size_rt = sizeof(data->req_type)/sizeof(data->req_type[0]);
		extract_freq_ar(&(data->req_type[0]), size_rt, result, req_type_cfl_add);
	}

	if(data->rsp_status_active) {
		cfl_init(&(result->rsp_status));
		int size_rs = sizeof(data->rsp_status)/sizeof(data->rsp_status[0]);
		extract_freq_ar(&(data->rsp_status[0]), size_rs, result, rsp_status_cfl_add);
	}

	return result;
}

void process_rate(Data* data) {
	if(data->req_rate.active) {
		int req_subtotal = data->req_rate.subtotal.value;
		update_metric_min(&(data->req_rate), req_subtotal);
		update_metric_max(&(data->req_rate), req_subtotal);
		add_metric_total(&(data->req_rate), req_subtotal);
		reset_metric_subtotal(&(data->req_rate));
	}

	if(data->err_rate.active) {
		double err_rate_subtotal = get_err_rate_subtotal();
		update_metric_min(&(data->err_rate), err_rate_subtotal);
		update_metric_max(&(data->err_rate), err_rate_subtotal);
		add_metric_total(&(data->err_rate), data->err_rate.subtotal.value);
		reset_metric_subtotal(&(data->err_rate));
	}

	if(data->conn_rate.active) {
		int conn_int_cnt = data->conn_ht.int_cnt;
		add_metric_sum(&(data->conn_rate), conn_int_cnt);
		update_metric_min(&(data->conn_rate), conn_int_cnt);
		update_metric_max(&(data->conn_rate), conn_int_cnt);
		
		// reset interval (1 sec) counter
		reset_int_cnt(&(data->conn_ht));
	}

	// TODO: For each loop for all hashtable
	reset_int_cnt(&(data->client_ht));
	reset_int_cnt(&(data->req_path_ht));

	if(data->tp.active) {
		if(data->tp.total.value > 0) {
			u_int32_t new_sec_ref_value = data->tp.total.value;
			u_int32_t tp_subtotal = new_sec_ref_value - data->sec_ref_value;
			data->sec_ref_value = new_sec_ref_value;
			
			update_metric_min(&(data->tp), tp_subtotal);
			update_metric_max(&(data->tp), tp_subtotal);	
		}
	}

	if(data->tp_rev.active) {
		if(data->tp_rev.total.value > 0) {
			u_int32_t new_sec_rev_ref_value = data->tp_rev.total.value;
			u_int32_t tp_rev_subtotal = new_sec_rev_ref_value - data->sec_rev_ref_value;
			data->sec_rev_ref_value = new_sec_rev_ref_value;
			
			update_metric_min(&(data->tp_rev), tp_rev_subtotal);
			update_metric_max(&(data->tp_rev), tp_rev_subtotal);	
		}	
	}
}

void flow_hash_process() {
	Data* data = {0};
	get_data(&data);
	int i = 0;
	flow_t *flow, *flow_next = NULL;

	for(i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(data->flow_hash_table[i]->mutex));
		flow = data->flow_hash_table[i]->first;
		
		while(flow != NULL ){
			flow_next = flow->next;
			flow_extract_http(flow, FALSE);
			extract_data(flow);
			flow = flow_next;
		}
		
		pthread_mutex_unlock(&(data->flow_hash_table[i]->mutex));
	}
}

void free_result(Result* result) {
	Data* data = {0};
	get_data(&data);

	if(data->client_active) {
		cfl_delete(&(result->client));
	}

	if(data->req_path_active) {
		cfl_delete(&(result->req_path));
	}
	
	if(data->req_method_active) {
		cfl_delete(&(result->req_method));
	}
	
	if(data->req_type_active) {
		cfl_delete(&(result->req_type));
	}
	
	if(data->rsp_status_active) {
		cfl_delete(&(result->rsp_status));
	}
	
	free(result);
}

void process_data(int sig) {
	Data* data = {0};
	get_data(&data);

	data->int_step++;

	// The completed flow are processed by extract_data every seconds
	// We process the ones in the hash table using the following function 	
	flow_hash_process();

	data->analysis->eventManager->timerExpired->notify();
	
	pthread_mutex_lock(&(data->lock));
	process_rate(data);
	data->stamp++;	

	data->rsp_sec_tot = 0;

	if(data->int_step < data->interval) {
		pthread_mutex_unlock(&(data->lock));
		return;
	}

	data->analysis->eventManager->intervalExpired->notify();

	Result* result = get_result();
	reset(data);
	data->rsp_int_tot = 0;
	pthread_mutex_unlock(&(data->lock));

	if(debug) {
		print_data(result);
		fflush(stdout);
	}

	if(data->status == -1) {
		//timer_delete(tid);
		//print_conn_tl(&(data->conn_ht.tl));

		if(debug) {
			printf("pak: %d \n", pak);
			printf("dpak: %d \n", pak_deq);
			printf("reqn: %d \n", req_n);
			printf("rspn: %d \n", rsp_n);
		}
	}

	// Struct are passed by value so the code below will execute correctly in an MT env
	if(is_server_mode()) {
		send_data(result);
	} 

	free_result(result);
	
	if(data->status < 0) {
		pthread_exit(NULL);		
	}
}

void print_data(Result* result) {
	Data* data = {0};
	get_data(&data);

	time_t raw_time;
	struct tm *timeinfo = NULL;
	char time_buf[20];
	
	// Get local time
	time( &raw_time );
	timeinfo = localtime( &raw_time );
	memset(time_buf, 0, sizeof(time_buf));
	strftime(time_buf, sizeof(time_buf), "%Y%m%d %H:%M:%S", timeinfo);

	printf("Time: %s \n", time_buf);
	
	if(data->rst.active) {
		printf("RST (avg, min, max): %f %f %f \n", result->rst_avg, result->rst_min, result->rst_max);
	}

	if(data->req_rate.active) {
		printf("REQ RATE (avg, min, max): %f %f %f \n", result->req_rate, result->req_rate_min, result->req_rate_max);
	}

	if(data->err_rate.active) {
		printf("ERR RATE (avg, min, max): %f %f %f \n", result->err_rate, result->err_rate_min, result->err_rate_max);
	}

	if(data->conn_rate.active) {
		printf("CONN RATE (avg, min, max): %f %f %f \n", result->conn_rate, result->conn_rate_min, result->conn_rate_max);
	}

	if(data->client_active) {
		printf("Client cumulated freq list \n");
		print_cfl(&(result->client));
		printf("\n");
	}

	if(data->req_path_active) {
		printf("Path cumulated freq list \n");
		print_cfl(&(result->req_path));
		printf("\n");
	}

	if(data->req_method_active) {
		printf("Method cumulated freq list \n");
		print_cfl(&(result->req_method));
		printf("\n");
	}

	if(data->req_type_active) {
		printf("Req Type cumulated freq list \n");
		print_cfl(&(result->req_type));
		printf("\n");
	}

	if(data->rsp_status_active) {
		printf("Rsp Status cumulated freq list \n");
		print_cfl(&(result->rsp_status));
		printf("\n");
	}
}