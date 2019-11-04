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

Client::Client(Analysis* analysis) 
:MetricCumDistr(analysis, "clients", "Clients"), reqTotal(0) {
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

void Client::onRequestReceived(http_pair_t *pair, flow_t *flow) {
	reqTotal++;

	//request_t *req = pair->request_header;
	u_int32_t* addr_ = CALLOC(u_int32_t, 1);
	*addr_ = flow->socket.saddr;
	Addr2* addr = new Addr2(addr_);

	ht->add(addr);
}

void Client::onIntervalExpired() {
	cflUpdate(ht);
	sendMsg();
	print();
	cfl_delete(&cfl);
}

void Client::onNewFlowReceived(flow_t *flow) {
}

void Client::onFlowUpdate(flow_t *flow) {
}

void Client::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void Client::onTimerExpired() {
}