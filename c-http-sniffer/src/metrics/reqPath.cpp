#include "reqPath.hpp"

ReqPath::ReqPath(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricCumDistr(protocol, analysis, "req_path", "Requests path"), reqTotal(0) {
	ht = new Hash();
}

void ReqPath::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->intervalExpired->add(this);
}

void ReqPath::cflAdd(Hashable* elem, int cnt) {
	if(reqTotal <= 0) return;
	double freq = (double) cnt / reqTotal;

	if(freq > MIN_FREQ) {
		// Item is freed on cfl_delete so we need a copy
		char* reqPath = strdup((char*)elem->value);
		cfl_add(reqPath, freq, &cfl);	
	}
}

void ReqPath::cflAdd(int i, int cnt) {
}

void ReqPath::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
	reqTotal++;
	const char *uri = ((_http::Request*)pair->request_header)->uri;
	
	// URI is NULL when req->method == NONE
	if(uri != NULL) {
		char* reqPath = extractReqPath(uri);
		StringHash* path = new StringHash(reqPath);
		ht->add(path);
	}
}

char* ReqPath::extractReqPath(const char* uri) {
    char* reqPath;
	std::string str(uri);

	size_t last = str.find_last_of('/'); 
	if (last == std::string::npos) {
		size_t query = str.find('?');
		std::string substr = str.substr(0, query);

		reqPath = CALLOC(char, substr.length()+1);
  		strcpy(reqPath, substr.c_str());
	} else {
		std::string substr = str.substr(0, last+1);
		reqPath = CALLOC(char, substr.length()+1);
  		strcpy(reqPath, substr.c_str());
	}

    return reqPath;
}

void ReqPath::onIntervalExpired() {
	cflUpdate(ht);
	if(analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void ReqPath::onAnalysisEnded() {
}

void ReqPath::onNewFlowReceived(Flow* flow) {
}

void ReqPath::onFlowUpdate(Flow* flow) {
}

void ReqPath::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
}

void ReqPath::onTimerExpired() {
}