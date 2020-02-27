#include "reqType.hpp"
#include "http.hpp"

ReqType::ReqType(Protocol* protocol, Analysis* analysis) 
: MetricCumDistr(protocol, analysis, "req_type", "Request type"), reqTotal(0) {
	reqTypeSize = ((Http*)protocol)->getMethodCount();
	reqType = new int[reqTypeSize]{};
}

void ReqType::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->intervalExpired->add(this);
}

void ReqType::cflAdd(Hashable* elem, int cnt) {
}

void ReqType::cflAdd(int index, int cnt) {
	if(reqTotal <= 0) return;
	double freq = (double) cnt / reqTotal;

	if(freq > MIN_FREQ) {
        // Item is freed on cfl_delete so we need a copy
        char* type = protocol->getMethodName(index);
        char* reqType = strdup(type);
		cfl_add(reqType, freq, &cfl);	
	}
}

void ReqType::onRequestReceived(Pair *pair, Flow* flow) {
	reqTotal++;
    Method type = ((_http::Request*)pair->request_header)->method;
    int* cnt = &(reqType[type]);
    __atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

void ReqType::onIntervalExpired() {
    cflUpdate(reqType, reqTypeSize);
	if(analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void ReqType::onAnalysisEnded() {
}

void ReqType::onNewFlowReceived(Flow* flow) {
}

void ReqType::onFlowUpdate(Flow* flow) {
}

void ReqType::onResponseReceived(Pair *pair, Flow* flow) {
}

void ReqType::onTimerExpired() {
}