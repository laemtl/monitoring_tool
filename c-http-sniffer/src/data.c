#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/syscall.h>
#include <inttypes.h>
#include <float.h>
#include "data.h"
#include "path.h"

extern int flow_req;
extern int flow_rsp;

int d = 0;

/* Output flow's brief to stdout */
/** Added Functionality: Function now takes file descriptor as arguement
 *  file descriptor should point to sockt in remote node where
 *  logger is listening
**/

static pthread_key_t data;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

void destr_fn(void *param) {
   //printf("Destructor function invoked\n");
   //free(param);
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
	metric->sum.value = 0;
	metric->min.value = DBL_MAX;
	metric->max.value = 0;
}

void reset(Data* data) {
	reset_metric(&(data->rst));
	reset_metric(&(data->err_rate));
	reset_metric(&(data->req_rate));
	reset_metric(&(data->tp));
	reset_metric(&(data->conn_rate));

	data->int_step = 0;

	// TODO: clean on exit
	//hash_clear(&(data->client_ht)); 
}

Data* init_data() {
	Data* data = CALLOC(Data, 1);
		
	init_once_metric(&(data->rst));
	init_once_metric(&(data->err_rate));
	init_once_metric(&(data->req_rate));
	init_once_metric(&(data->tp));

	reset(data);	
	data->status = 0;

	init_conn(data);
    init_client(data);
	init_path(data);

	return data;
};
	
static void thread_key_setup() {
	int status;
    if ((status = pthread_key_create(&data, destr_fn )) < 0) {
        printf("pthread_key_create failed, errno=%d", errno);
    	return EXIT_FAILURE;
    }
}

void thread_init(Data* d) {
    pthread_once(&init_done, thread_key_setup);

	if (pthread_setspecific(data, d) <  0) {
		printf("pthread_setspecific failed, errno %d", errno);
		pthread_exit((void *)12);
	}
}

void print_tid() {
	pid_t x = syscall(__NR_gettid);
	printf("pid: %ld \n", (long)x);
}

void get_data(Data** d) {	
	*d = (Data *)pthread_getspecific(data);
    if(d == NULL) {
        printf("pthread_getspecific failed, errno %d\n", errno);
        return EXIT_FAILURE;
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

	int req_total = data->req_rate.total.value; 
	if (req_total > 0) return data->err_rate.total.value/req_total;
	return 0;
}

// Get error rate with subtotals
double get_err_rate_subtotals() {
	Data* data = {0};
	get_data(&data);

	int req_subtotal = data->req_rate.subtotal.value; 
	if (req_subtotal > 0) return data->err_rate.subtotal.value/req_subtotal;
	return 0;
}

double get_req_rate() {
	Data* data = {0};
	get_data(&data);

	int req_total = data->req_rate.total.value;
	return (double)req_total/data->interval;
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

void inc_metric_total(Metric* metric) {
    pthread_mutex_lock(&(metric->total.mutex));
	metric->total.value++;
	pthread_mutex_unlock(&(metric->total.mutex));
}

void inc_metric_subtotal(Metric* metric) {
	pthread_mutex_lock(&(metric->subtotal.mutex));
	metric->subtotal.value++;
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

	if(!flow->processed) {
		add_conn(flow->socket.saddr, flow->socket.sport, data);
		add_client(flow->socket.saddr, data);
		flow->processed = TRUE;				
	}

	if (flow->http_f != NULL){        	
		http_pair_t *h = flow->http_f;
		while(h != NULL) {

			if(h->request_header != NULL) {
				if(!h->req_processed) {
					h->req_processed = TRUE;
					request_t *req = h->request_header;
					flow_req++;

					add_path(req->uri, data);
					compute_req_rate(data);
				}
				
				if(h->response_header != NULL && !h->rsp_processed) {
					h->rsp_processed = TRUE;
					flow_rsp++;

					compute_rst(h, data);
					response_t *rsp = h->response_header;					
					compute_err_rate(rsp, data);
				}
				
				if(h->response_header == NULL) d++;
			}	   
			h = h->next;
		}
	}
}

void compute_req_rate(Data* data) {
	inc_metric_subtotal(&(data->req_rate));
	inc_metric_total(&(data->req_rate));
}

void compute_rst(http_pair_t *h, Data* data) {
	// Compute response time
	double rst = (h->rsp_fb_sec + h->rsp_fb_usec * 0.000001) - (h->req_fb_sec + h->req_fb_usec * 0.000001);

	response_t *rsp = h->response_header;
	request_t *req = h->request_header;

	if(rst < 0 ) {
		//printf("req_seq: %" PRIu32"\n", req->seq);
		printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
		printf("aknowledgment: %ld \n", rsp->acknowledgement);

		//printf("req %lld %lld \n", (long long)h->req_fb_sec, (long long)h->req_fb_usec);
		//printf("rsp %lld %lld \n", (long long)h->rsp_fb_sec, (long long)h->rsp_fb_usec);
	}

	//int res = tcp_order_check(f->order);
	//if(res == 0) printf("Unordered flow \n");
	
	inc_metric_total(&(data->rst));
	add_metric_sum(&(data->rst), rst);
	update_metric_min(&(data->rst), rst);
	update_metric_max(&(data->rst), rst);
}

void compute_err_rate(response_t *rsp, Data* data) {
	// Extract first digit of status code
	int i = rsp->status;
	while (i>=10) i=i/10;  
	if (i==4 || i==5) {
		inc_metric_subtotal(&(data->err_rate));
		inc_metric_total(&(data->err_rate));
	}
}

// TODO: check if status active
Result* get_result(Result* result) {
    Data* data = {0};
	get_data(&data);

	result->interface = data->interface;	
	result->client_sock = data->client_sock;

	result->rst_avg = get_rst_avg();
	result->rst_min = get_metric_min(data->rst);
    result->rst_max = data->rst.max.value;
	
	result->err_rate = get_err_rate();    
	result->err_rate_min = get_metric_min(data->err_rate);   
	result->err_rate_max = data->err_rate.max.value;   
	
	result->req_rate = get_req_rate(); 
	result->req_rate_min = get_metric_min(data->req_rate);
	result->req_rate_max = data->req_rate.max.value;

	result->conn_rate = get_conn_rate();
	result->conn_rate_min = data->conn_rate.min.value;
	result->conn_rate_max = data->conn_rate.max.value;
	
	result->path_avg = data->path_ht.int_cnt;
	
	return result;
}

// TODO: check if status active
void process_rate(Data* data) {
	int req_subtotal = data->req_rate.subtotal.value;
	update_metric_min(&(data->req_rate), req_subtotal);
	update_metric_max(&(data->req_rate), req_subtotal);
	
	double err_rate_subtotals = get_err_rate_subtotals();
	update_metric_min(&(data->err_rate), err_rate_subtotals);
	update_metric_max(&(data->err_rate), err_rate_subtotals);

	int conn_int_cnt = data->conn_ht.int_cnt;
	add_metric_sum(&(data->conn_rate), conn_int_cnt);
	update_metric_min(&(data->conn_rate), conn_int_cnt);
	update_metric_max(&(data->conn_rate), conn_int_cnt);
	
	reset_metric_subtotal(&(data->req_rate));
	reset_metric_subtotal(&(data->err_rate));	
	reset_metric_subtotal(&(data->conn_rate));	
}

void flow_hash_process() {
	Data* data = {0};
	get_data(&data);
	int i = 0;
	flow_t	*flow, *flow_next = NULL;

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

	return 0;
}

void process_data() {
	Data* data = {0};
	get_data(&data);

	data->int_step++;

	// The completed flow are processed by extract_data
	// We process the ones in the hash table using the following function 	
	flow_hash_process();
	
	process_rate(data);
	data->stamp++;	

	reset_int_cnt(&(data->conn_ht));
	reset_int_cnt(&(data->client_ht));
	reset_int_cnt(&(data->path_ht));
		
	if(data->int_step < data->interval) return;

	Result* result = calloc(1, sizeof(Result));
	get_result(result);

#if DEBUGGING == 2
	print_data(result);
	fflush(stdout);
#endif

	if(data->status == -1) {
		print_conn_tl(&(data->conn_ht.tl));
		print_client_tl(&(data->client_ht.tl));
		print_path_tl(&(data->path_ht.tl));
		//clear_tl();
	}

	reset(data);

	// Struct are passed by value so the code below will execute correctly in an MT env
	if(is_server_mode()) {
		send_data(result);
	} 

	free(result);
	if(data->status < 0) pthread_exit(NULL);		
}

void print_data(Result* result) {
	time_t raw_time;
	struct tm *timeinfo = NULL;
	char time_buf[20];
	
	// Get local time
	time( &raw_time );
	timeinfo = localtime( &raw_time );
	memset(time_buf, 0, sizeof(time_buf));
	strftime(time_buf, sizeof(time_buf), "%Y%m%d %H:%M:%S", timeinfo);

	printf("%s - %f %f %f %f %f %f %f %f %f %f %f %f %f \n", 
		time_buf, 
		result->rst_avg, result->rst_min, result->rst_max, 
    	result->req_rate, result->req_rate_min, result->req_rate_max,
        result->err_rate, result->err_rate_min, result->err_rate_max,
		result->conn_rate, result->conn_rate_min, result->conn_rate_max,
		result->path_avg
    );
}

// Cumulated frequency of requested objects
// For each objects, a different line
// total number of requested object
// 5 more popular
//obj_total / req_total 