#include "rspStatus.hpp"

RspStatus::RspStatus(Analysis* analysis)
: MetricCumDistr(analysis, "rsp_status", "Response status"), rspTotal(0) {
	rspStatusSize = sizeof(rspStatus)/sizeof(rspStatus[0]);
}

void RspStatus::subscribe(EventManager* em) {
	em->responseReceived->add(this);
	em->intervalExpired->add(this);
}

void RspStatus::cflAdd(Hashable* elem, int cnt) {
}

void RspStatus::cflAdd(int index, int cnt) {
	if(rspTotal <= 0) return;
	double freq = (double) cnt / rspTotal;

	if(freq > MIN_FREQ) {
        // Item is freed on cfl_delete so we need a copy
		char* status = CALLOC(char, 4);
        sprintf(status, "%d", index);
        cfl_add(status, freq, &cfl);	
	}
}

void RspStatus::onRequestReceived(pair_t *pair, Flow* flow) {
}

void RspStatus::onIntervalExpired() {
	cflUpdate(rspStatus, rspStatusSize);
	if(analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void RspStatus::onAnalysisEnded() {
}

void RspStatus::onNewFlowReceived(Flow* flow) {
}

void RspStatus::onFlowUpdate(Flow* flow) {
}

void RspStatus::onResponseReceived(pair_t *pair, Flow* flow) {
	rspTotal++;
    
    // Atomic increment
    response_t *rsp = pair->response_header;					
    int status = rsp->status;

    int* cnt = &(rspStatus[status]);
	__atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

void RspStatus::onTimerExpired() {
}