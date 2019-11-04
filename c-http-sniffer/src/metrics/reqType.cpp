#include "reqType.hpp"

ReqType::ReqType(Analysis* analysis) 
: MetricCumDistr(analysis, "req_type", "Request type"), reqTotal(0) {
	reqTypeSize = sizeof(reqType)/sizeof(reqType[0]);
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
        char* type = HTTP_METHOD_STRING_ARRAY[index];
        char* reqType = strdup(type);
		cfl_add(reqType, freq, &cfl);	
	}
}

void ReqType::onRequestReceived(http_pair_t *pair, flow_t *flow) {
	reqTotal++;
    http_mthd type = pair->request_header->method;
    int* cnt = &(reqType[type]);
    __atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

void ReqType::onIntervalExpired() {
    cflUpdate(reqType, reqTypeSize);
	sendMsg();
	print();
	cfl_delete(&cfl);
}

void ReqType::onNewFlowReceived(flow_t *flow) {
}

void ReqType::onFlowUpdate(flow_t *flow) {
}

void ReqType::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void ReqType::onTimerExpired() {
}