#include "analysis.hpp"
#include <inttypes.h>
#include <libgen.h>

void rsp_status_cfl_add(int i, int cnt, Result* r) {
    Data* data = {0};
	get_data(&data);
    //char* rsp_status;

	if(data->req_tot <= 0) return;
		
	double freq = (double) cnt / data->rsp_tot;
    if(freq > MIN_FREQ) {
        char* rsp_status = CALLOC(char, 4);
        sprintf(rsp_status, "%d", i);
        cfl_add(rsp_status, freq, &(r->rsp_status));
    }
}

// Atomic increment
void add_rsp_status(int rsp_status, Data* data) {
    if(!data->rsp_status_active) return;
    
    int* cnt = &(data->rsp_status[rsp_status]);  
	__atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}