#include "req_path.h";
#include "data.h";
#include <inttypes.h>
#include <libgen.h>

void req_type_cfl_add(int i, int cnt, Result* r) {
    Data* data = {0};
	get_data(&data);
    char* rsp_status;

	if(data->req_tot <= 0) return;
		
	double freq = (double) cnt / data->req_tot;
    if(freq > MIN_FREQ) {
        char* type = HTTP_METHOD_STRING_ARRAY[i];
        char* req_type = strdup(type);
	    cfl_add(req_type, freq, &(r->req_type));
    }
}

// Atomic increment
void add_req_type(http_mthd type, Data* data) {
    if(!data->req_type_active) return;
    
    int* cnt = &(data->req_type[type]);  
	__atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}