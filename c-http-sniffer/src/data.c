#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/syscall.h>
#include <inttypes.h>
#include <float.h>
#include "data.h"

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
	pthread_mutex_init(&(metric->min.mutex), NULL);
	pthread_mutex_init(&(metric->max.mutex), NULL);
}

void reset_metric(Metric* metric) {
	metric->total.value = 0;
	metric->min.value = DBL_MAX;
	metric->max.value = 0;
}

void reset(Data* data) {
	reset_metric(&(data->rst));
	reset_metric(&(data->err_rate));
	reset_metric(&(data->req_rate));
	reset_metric(&(data->tp));

	data->int_step = 0;
	hash_clear(data->clients_ht); 
}

void init_data(Data* data) {
	init_once_metric(&(data->rst));
	init_once_metric(&(data->err_rate));
	init_once_metric(&(data->req_rate));
	init_once_metric(&(data->tp));

	reset(data);	
	data->status = 0;

	client_init(data);
}

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
	    return -1;
    }
	return metric.min.value;
}

double get_rst_avg(Metric metric) {
	Data* data = {0};
	get_data(&data);

	double req_total = data->req_rate.total.value; 
	if (!CLOSE(req_total, 0)) return metric.total.value/req_total;
	return 0;
}

double get_err_rate() {
	Data* data = {0};
	get_data(&data);

	double req_total = data->req_rate.total.value; 
	if (req_total > 0) return data->err_rate.total.value/req_total;
	return 0;
}

// Get error rate with subtotals
double get_err_rate_subtotals() {
	Data* data = {0};
	get_data(&data);

	double req_subtotal = data->req_rate.subtotal.value; 
	if (req_subtotal > 0) return data->err_rate.subtotal.value/req_subtotal;
	return 0;
}

double get_req_rate() {
	Data* data = {0};
	get_data(&data);

	double req_total = data->req_rate.total.value;
	return (double)req_total/data->interval;
}

void inc_metric_total(Metric* metric, double amt) {
    pthread_mutex_lock(&(metric->total.mutex));
	metric->total.value += amt;
	pthread_mutex_unlock(&(metric->total.mutex));
}

void inc_metric_subtotal(Metric* metric, double amt) {
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

bool is_server_mode() {
    Data* data = {0};
	get_data(&data);
	return data->server_mode;
}

void extract_data(const flow_t *flow){	
	Data* data = {0};
	get_data(&data);

	/*Convert IP addr */
	//char *saddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	//char *daddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	
	int n = sizeof("aaa.bbb.ccc.ddd") + 1;
	char *saddr[n];
	char *daddr[n];
	strncpy(saddr, ip_ntos(flow->socket.saddr), n);
	strncpy(daddr, ip_ntos(flow->socket.daddr), n);
	saddr[n] = '\0';
	daddr[n] = '\0';
    
	if (flow->http_f != NULL){        	
		http_pair_t *h = flow->http_f;
		while(h != NULL) {

			if(h->request_header != NULL) {
				request_t *req = h->request_header;
				
				if(h->response_header != NULL) {
					response_t *rsp = h->response_header;

					// Compute response time
					double rst = (h->rsp_fb_sec + h->rsp_fb_usec * 0.000001) - (h->req_fb_sec + h->req_fb_usec * 0.000001);

					inc_metric_total(&(data->rst), rst);
					update_metric_min(&(data->rst), rst);
					update_metric_max(&(data->rst), rst);
					
					// Extract first digit of status code
					int i=rsp->status;
					while (i>=10) i=i/10;  
					if (i==4 || i==5) {
						inc_metric_subtotal(&(data->err_rate), 1);
						inc_metric_total(&(data->err_rate), 1);
					}
				}
			}	   
			h = h->next;
		}
	}
}


// TODO: check if status active
Result* get_result(Result* result) {
    Data* data = {0};
	get_data(&data);

	result->interface = data->interface;	
	result->client_sock = data->client_sock;

	result->rst_avg = get_rst_avg(data->rst);
	result->rst_min = get_metric_min(data->rst);
    result->rst_max = data->rst.max.value;
	
	result->err_rate = get_err_rate();    
	result->err_rate_min = get_metric_min(data->err_rate);   
	result->err_rate_max = data->err_rate.max.value;   
	
	result->req_rate = get_req_rate(); 
	result->req_rate_min = get_metric_min(data->req_rate);
	result->req_rate_max = data->req_rate.max.value;

	result->clients_tot = data->clients_ht->cnt;
	
	return result;
}

// TODO: check if status active
void process_rate(Data* data) {
	double req_subtotal = data->req_rate.subtotal.value;
	update_metric_min(&(data->req_rate), req_subtotal);
	update_metric_max(&(data->req_rate), req_subtotal);
	
	double err_rate_subtotals = get_err_rate_subtotals();
	update_metric_min(&(data->err_rate), err_rate_subtotals);
	update_metric_max(&(data->err_rate), err_rate_subtotals);
	
	reset_metric_subtotal(&(data->req_rate));
	reset_metric_subtotal(&(data->err_rate));	
}

void process_data() {
	Data* data = {0};
	get_data(&data);
	
	data->int_step++;
	process_rate(data);
	if(data->int_step < data->interval) return;

	Result* result = calloc(1, sizeof(Result));
	get_result(result);
	reset(data);

#if DEBUGGING == 2
	print_data(result);
	fflush(stdout);
#endif

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

	printf("%s - %f %f %f %f %f %f %f %f %f %f %d \n", time_buf, 
		result->rst_avg, result->rst_min, result->rst_max, 
    	result->req_rate, result->req_rate_min, result->req_rate_max,
        result->err_rate, result->err_rate_min, result->err_rate_max,
		result->clients_tot
    );
}

// install online

// Cumulated frequency of requested objects
// For each objects, a different line
// total number of requested object
// 5 more popular

// save DS and counter

/*void parseURI(const char *line) {
	start_of_path=strchr(line, '?');
	len = start_of_path-line;
	path = malloc(sizeof(char) * (len + 1));
	
	if ( NULL == path ){ 
        free(path);
        return;
	} else {        
        (void)strncpy(path, line, len);
        path[len] = '\0';
	}
    
	//  printf("%s\n", path);
    start_of_path++;
    end_of_query=strchr(start_of_path, '\0');
    //printf("%s\n",end_of_query);
    len=end_of_query-start_of_path;
	query=malloc(sizeof(char) * (len + 1));
	
	if ( NULL == query ) {
		free(query);
	}
    
	(void)strncpy(query, start_of_path, len);
    query[len] = '\0';
}*/
