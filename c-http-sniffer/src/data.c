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

void init_data(Data* data) {
	pthread_mutex_init(&(data->lock), NULL);
	data->running = TRUE;
	data->rst_min = DBL_MAX;
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

	//init();
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

void* inc_req_t() {
    Data* data = {0};
	get_data(&data);
    pthread_mutex_lock(&data->lock);
	data->req_t++;
	pthread_mutex_unlock(&data->lock); 
}

void* inc_err_t() {
    Data* data = {0};
	get_data(&data);
    pthread_mutex_lock(&data->lock);
	data->err_t++;
	pthread_mutex_unlock(&data->lock); 
}    

void* inc_rst_t(double rst_t) {
    Data* data = {0};
	get_data(&data);
    pthread_mutex_lock(&data->lock);
	data->rst_t += rst_t;
	pthread_mutex_unlock(&data->lock); 
}

void* update_rst_min(double rst) {
    Data* data = {0};
	get_data(&data);

	// TODO: lock are not efficient. Doesn't allow 2 concurent different writes
	// We can probaly remove them, as only one thread write these values 
	pthread_mutex_lock(&data->lock);
	if(rst < data->rst_min) 
		data->rst_min = rst;
	pthread_mutex_unlock(&data->lock); 
}

void* update_rst_max(double rst) {
    Data* data = {0};
	get_data(&data);

	// TODO: lock are not efficient. Doesn't allow 2 concurent different writes
	// We can probaly remove them, as only one thread write these values 
	pthread_mutex_lock(&data->lock);
	if(rst > data->rst_max) 
		data->rst_max = rst;
	pthread_mutex_unlock(&data->lock); 
}

Result* get_result(Result* result) {
    Data* data = {0};
	get_data(&data);

	result->interface = data->interface;	
	result->client_sock = data->client_sock;

	if (data->rst_t > 0){
		result->rst_avg = data->rst_t/data->req_t;
		result->errrate = data->err_t/data->req_t;
	}    
    
    if( data->rst_min == DBL_MAX) {
	    result->rst_min = -1;
    } else {
	    result->rst_min = data->rst_min;
    }	
    result->rst_max = data->rst_max;

	result->hl = (double)data->req_t/data->interval;
	result->tp = (double)(data->req_t - data->err_t)/data->interval;
	
	init();

	return result;
}

void init () {
    Data* data = {0};
	get_data(&data);
    data->req_t = 0;
    data->err_t = 0;
    data->rst_t = 0;
	//data->rst_min = DBL_MAX;
	//data->rst_max = 0;
}

bool is_server_mode() {
    Data* data = {0};
	get_data(&data);
	return data->server_mode;
}

// TODO: add an array like DS to compute avg per source/dest?
// TODO: Rename to flow_save (don't print anymore)
// TODO: Be sure to lock before editing global variable
void extract_data(const flow_t *flow){	
	/*Convert IP addr */
	//TODO: Check if malloc is necessary
	//char *saddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	//char *daddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	
	/*int n = sizeof("aaa.bbb.ccc.ddd") + 1;
	char *saddr[n];
	char *daddr[n];
	strncpy(saddr, ip_ntos(flow->socket.saddr), n);
	strncpy(daddr, ip_ntos(flow->socket.daddr), n);
	saddr[n] = '\0';
	daddr[n] = '\0';*/
    
	if (flow->http_f != NULL){        	
		http_pair_t *h = flow->http_f;
    	
		while(h != NULL){
			inc_req_t();
			if(h->request_header != NULL) {
				request_t *req = h->request_header;
				
				if(h->response_header != NULL) {
					response_t *rsp = h->response_header;

					// Compute response time
					double rst = ((h->rsp_fb_sec + h->rsp_fb_usec) - (h->req_fb_sec + h->req_fb_usec)) * 0.000001;

					inc_rst_t(rst);
					update_rst_max(rst);
					update_rst_min(rst);

					// Extract first digit of status code
					int i=rsp->status;
					while (i>=10) i=i/10;  
					if (i==4 || i==5) inc_err_t();
				}
			}	   
			h = h->next;
		}
	}
}

void process_data() {
	Data* data = {0};
	get_data(&data);
	
	Result* result = calloc(1, sizeof(Result));
	get_result(result);

#if DEBUGGING == 2
	print_data(result);
	fflush(stdout);
#endif

	// TODO: Callback with registered events??
	// Struct are passed by value so the code below will execute correctly in an MT env
	if(is_server_mode()) {
		send_data(result);
	} 

	free(result);
	if(!data->running) pthread_exit(NULL);
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

	printf("%s - %f %f %f %f \n", time_buf, result->rst_avg, result->rst_min, result->rst_max, result->errrate);
}

// install online

// Cumulated frequency of requested objects
// For each objects, a different line
// total number of requested object
// 5 more popular

// save DS and counter

/*void parseURI(const char *line)
{
  start_of_path=strchr(line, '?');
  len= start_of_path-line;
  path=malloc(sizeof(char) * (len + 1));
  if ( NULL == path ){ 
                   free(path);
                   return;
}
             else
             {        
               (void)strncpy(path, line, len);
               path[len] = '\0';}
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
