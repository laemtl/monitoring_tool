#include <inttypes.h>
#include <libgen.h>

#include "metric.hpp"

class RspStatus 
{
	private:
		//Metric<double> rspStatus;
        int statusStats[599];
		int rsp_tot;

	public:
		double rspStatus();
		//double getAvg();
		void onResponseReceived(http_pair_t *h, Flow *flow);
		void print();
}

void rsp_status_cfl_add(int i, int cnt, Result* r) {
    //char* rsp_status;

	if(rsp_tot <= 0) return;
		
	double freq = (double) cnt / rsp_tot;
    if(freq > MIN_FREQ) {
        char* rsp_status = CALLOC(char, 4);
        sprintf(rsp_status, "%d", i);
        cfl_add(rsp_status, freq, &(r->rsp_status));
    }
}


void RspStatus::onResponseReceived(http_pair_t *h, Flow *flow) {

    if(!isActive()) return;

	rsp_tot++;
    
    // Atomic increment
    response_t *rsp = h->response_header;					
    int status = rsp->status;

    int* cnt = statusStats[status];  
	__atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

if(rsp_status_active) {
	cfl_init(&(result->rsp_status));
	int size_rs = sizeof(data->rsp_status)/sizeof(data->rsp_status[0]);
	extract_freq_ar(&(data->rsp_status[0]), size_rs, result, rsp_status_cfl_add);
}

cfl_delete(&(result->rsp_status));