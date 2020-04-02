#include "reqType.hpp"
#include "requestMethod.hpp"

ReqType::ReqType(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricCumDistr(protocol, analysis, "req_type", "Request type"), reqTotal(0) {
}

void ReqType::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->intervalExpired->add(this);
}

void ReqType::cflAdd(Hashable* elem, int cnt) {
}

void ReqType::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
	reqTotal++;

	RequestMethod* req = (RequestMethod*)pair->request_header;
	
	int methodCode = req->methodCode;	
	printf("methodCode in reqType %d \n", req->getMethodCode());
	
	char* methodName = req->getMethodName(methodCode);
    if(methodName != NULL) ++reqType[methodName];
}

void ReqType::onIntervalExpired() {
	if(reqTotal <= 0) return;
	
	for ( it = reqType.begin(); it != reqType.end(); it++ ) {
    	int cnt = it->second;
		double freq = (double) cnt / reqTotal;

		if(freq > MIN_FREQ) {
			// Item is freed on cfl_delete so we need a copy
			char* reqMethod = strdup((char*)it->first);
			cfl_add(reqMethod, freq, &cfl);	
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

void ReqType::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
}

void ReqType::onTimerExpired() {
}
