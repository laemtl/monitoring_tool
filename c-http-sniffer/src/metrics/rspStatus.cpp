#include "rspStatus.hpp"
#include "responseStatus.hpp"

RspStatus::RspStatus(_protocol::Protocol* protocol, Analysis* analysis)
: MetricCumDistr(protocol, analysis, "rsp_status", "Response status"), rspTotal(0) {
}

void RspStatus::subscribe(EventManager* em) {
	em->responseReceived->add(this);
	em->intervalExpired->add(this);
}

void RspStatus::cflAdd(Hashable* elem, int cnt) {
}

void RspStatus::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
}

void RspStatus::onIntervalExpired() {
	if(rspTotal <= 0) return;
	
	for ( it = rspStatus.begin(); it != rspStatus.end(); it++ ) {
    	int cnt = it->second;
		double freq = (double) cnt / rspTotal;

		if(freq > MIN_FREQ) {
			// Item is freed on cfl_delete so we need a copy
			char* rspStatus = strdup((char*)it->first);
			cfl_add(rspStatus, freq, &cfl);	
		}
	}

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

void RspStatus::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
	rspTotal++;
	
	ResponseStatus* rsp = (ResponseStatus*)pair->response_header;
    int statusCode = rsp->statusCode;
	printf("statusCode: %d", statusCode);
	char* statusName = rsp->getStatusName(statusCode);
	if(statusName != NULL) ++rspStatus[statusName];
}

void RspStatus::onTimerExpired() {
}
