#include "reqPath.hpp"
#include <inttypes.h>
#include <libgen.h>
#include <string.h>

// TODO: lock
int req_type[41];
int req_tot = 0;

void req_type_cfl_add(int i, int cnt, Result* r) {
    //char* rsp_status;

	if(req_tot <= 0) return;
		
	double freq = (double) cnt / req_tot;
    if(freq > MIN_FREQ) {
        char* type = HTTP_METHOD_STRING_ARRAY[i];
        char* req_type = strdup(type);
	    cfl_add(req_type, freq, &(r->req_type));
    }
}

void ReqType::print() {
	if(isActive()) {
		printf("Req Type cumulated freq list \n");
		print_cfl(&(result->req_type));
		printf("\n");
    }
}

void ReqType::onRequestReceived(http_pair_t *h, Flow *flow) {
    // Atomic increment
    if(!isActive()) return;
    
    req_tot++;
    http_mthd type = h->request_header->method;
    int* cnt = &(req_type[type]);  
    __atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

if(data->req_type_active) {
    cfl_init(&(result->req_type));
    int size_rt = sizeof(data->req_type)/sizeof(data->req_type[0]);
    extract_freq_ar(&(data->req_type[0]), size_rt, result, req_type_cfl_add);
}

cfl_delete(&(result->req_type));