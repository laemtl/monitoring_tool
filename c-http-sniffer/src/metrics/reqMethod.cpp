#include "reqMethod.hpp"

ReqMethod::ReqMethod(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricCumDistr(protocol, analysis, "req_method", "Request methods"), reqTotal(0) {
	ht = new Hash();
}

void ReqMethod::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->intervalExpired->add(this);
}

void ReqMethod::cflAdd(Hashable* elem, int cnt) {
	if(reqTotal <= 0) return;
	double freq = (double) cnt / reqTotal;

	if(freq > MIN_FREQ) {
		// Item is freed on cfl_delete so we need a copy
		char* reqMethod = strdup((char*)elem->value);
		cfl_add(reqMethod, freq, &cfl);	
	}
}

void ReqMethod::cflAdd(int i, int cnt) {
}

void ReqMethod::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
	reqTotal++;
	const char *uri = ((_http::Request*)pair->request_header)->uri;
	char* reqPath = extractReqMethod(uri);
	if(reqPath == NULL) return;
	StringHash* str = new StringHash(reqPath);
	ht->add(str);
}

char* ReqMethod::extractReqMethod(const char* uri) {
    if(uri == NULL) return NULL;
	
	char* reqMethod;
	std::string str(uri);

	size_t query = str.find('?'); 
	if (query == std::string::npos) {
		reqMethod = CALLOC(char, str.length()+1);
  		strcpy(reqMethod, str.c_str());
	} else {
		std::string substr = str.substr(0, query);
		reqMethod = CALLOC(char, substr.length()+1);
  		strcpy(reqMethod, substr.c_str());
	}

    return reqMethod;
}

void ReqMethod::onIntervalExpired() {
	cflUpdate(ht);
	if(analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void ReqMethod::onAnalysisEnded() {
}

void ReqMethod::onNewFlowReceived(Flow* flow) {
}

void ReqMethod::onFlowUpdate(Flow* flow) {
}

void ReqMethod::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
}

void ReqMethod::onTimerExpired() {
}