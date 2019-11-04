#include "reqPath.hpp"

ReqPath::ReqPath(Analysis* analysis) 
: MetricCumDistr(analysis, "req_path", "Requests path"), reqTotal(0) {
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
		//char* reqPath = CALLOC(char, strlen((char*)elem->value)+1);
	  	//strcpy(reqPath, (char*)elem->value);
		char* reqPath = strdup((char*)elem->value);
		cfl_add(reqPath, freq, &cfl);	
	}
}

void ReqPath::cflAdd(int i, int cnt) {
}

void ReqPath::onRequestReceived(http_pair_t *pair, flow_t *flow) {
	reqTotal++;
	const char *uri = pair->request_header->uri;
	char* reqPath = extractReqPath(uri);
	StringHash* path = new StringHash(reqPath);
	ht->add(path);
}

char* ReqPath::extractReqPath(const char* uri) {
    char* reqPath;
	string str(uri);

	size_t last = str.find_last_of('/'); 
	if (last == string::npos) {
		size_t query = str.find('?');
		string substr = str.substr(0, query);

		reqPath = CALLOC(char, substr.length()+1);
  		strcpy(reqPath, substr.c_str());
	} else {
		string substr = str.substr(0, last+1);
		reqPath = CALLOC(char, substr.length()+1);
  		strcpy(reqPath, substr.c_str());
	}

    return reqPath;
}

void ReqPath::onIntervalExpired() {
	cflUpdate(ht);
	sendMsg();
	print();
	cfl_delete(&cfl);
}

void ReqPath::onNewFlowReceived(flow_t *flow) {
}

void ReqPath::onFlowUpdate(flow_t *flow) {
}

void ReqPath::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void ReqPath::onTimerExpired() {
}