#include "reqType.hpp"
#include "RequestMethod.hpp"

ReqType::ReqType(Protocol* protocol, Analysis* analysis) 
: MetricCumDistr(protocol, analysis, "req_type", "Request type"), reqTotal(0) {
}

void ReqType::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->intervalExpired->add(this);
}

void ReqType::cflAdd(Hashable* elem, int cnt) {
}

void ReqType::cflAdd(int index, int cnt) {
}

void ReqType::onRequestReceived(Pair *pair, Flow* flow) {
	reqTotal++;

	RequestMethod* req = (RequestMethod*)pair->request_header;
	int methodCode = req->methodCode;	
	char* methodName = req->getMethodName(methodCode);
    ++reqType[methodName];
}

void ReqType::onIntervalExpired() {
	if(reqTotal <= 0) return;
	
	for ( it = reqType.begin(); it != reqType.end(); it++ ) {
    	char* value = it->first;
		int cnt = it->second;
		double freq = (double) cnt / reqTotal;

		if(freq > MIN_FREQ) {
			cfl_add(value, freq, &cfl);	
		}
	}

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