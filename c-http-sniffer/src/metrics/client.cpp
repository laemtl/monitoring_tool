#include "client.hpp"

Addr2::Addr2(u_int32_t* val) : Hashable((void*)val) {
}

int Addr2::compare(Hashable* elem) {
    if(*(u_int32_t*)value == *(u_int32_t*) (elem->value)) return 0;
    else if (value > elem->value) return 1;
    return -1;
}

u_int32_t Addr2::hash() {
    return *(u_int32_t*)value;
}

Client::Client(Protocol* protocol, Analysis* analysis) 
:MetricCumDistr(protocol, analysis, "clients", "Clients"), reqTotal(0) {
	ht = new Hash();
}

void Client::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	//em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void Client::cflAdd(Hashable* elem, int cnt) {
	if(reqTotal <= 0) return;
	
	double freq = (double) cnt / reqTotal;
    if(freq > MIN_FREQ) {
	    cfl_add(ip_ntos(*(u_int32_t*)elem->value), freq, &cfl);
	}
}

void Client::cflAdd(int i, int cnt) {
}

void Client::onRequestReceived(Pair *pair, Flow* flow) {
	reqTotal++;

	//Request *req = pair->request_header;
	u_int32_t* addr_ = CALLOC(u_int32_t, 1);
	*addr_ = flow->socket.saddr;
	Addr2* addr = new Addr2(addr_);

	ht->add(addr);
}

void Client::onIntervalExpired() {
	cflUpdate(ht);
	if(analysis->isServerMode()) sendMsg();
	print();
	cfl_delete(&cfl);
}

void Client::onAnalysisEnded() {
}

void Client::onNewFlowReceived(Flow* flow) {
}

void Client::onFlowUpdate(Flow* flow) {
}

void Client::onResponseReceived(Pair *pair, Flow* flow) {
}

void Client::onTimerExpired() {
}