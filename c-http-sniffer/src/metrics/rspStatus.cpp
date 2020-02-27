#include "rspStatus.hpp"
#include "ResponseStatus.hpp"

RspStatus::RspStatus(Protocol* protocol, Analysis* analysis)
: MetricCumDistr(protocol, analysis, "rsp_status", "Response status"), rspTotal(0), rspStatus(NULL) {
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

void RspStatus::onRequestReceived(Pair *pair, Flow* flow) {
}

void RspStatus::onIntervalExpired() {
	if(rspStatus == NULL) return;

	cflUpdate(rspStatus, rspStatusSize);
	if(protocol->analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void RspStatus::onAnalysisEnded() {
}

void RspStatus::onNewFlowReceived(Flow* flow) {
}

void RspStatus::onFlowUpdate(Flow* flow) {
}

void RspStatus::onResponseReceived(Pair *pair, Flow* flow) {
	rspTotal++;
	
	ResponseStatus* rsp = (ResponseStatus*)pair->response_header;

	if(rspStatus == NULL) {
		rspStatusSize = rsp->getStatusCount();
		rspStatus = new int[rspStatusSize]{};
	}

    // Atomic increment
    int status = rsp->statusCode;				
    int* cnt = &(rspStatus[status]);
	__atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

void RspStatus::onTimerExpired() {
}