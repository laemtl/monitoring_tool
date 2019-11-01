#include "client.hpp"

Client::Client() {
	name = "CLIENT NEW";
	ht = new Hash();
}

void Client::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	//em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void Client::cflAdd(Addr2* addr, int cnt) {
	if(req_tot <= 0) return;
	
	double freq = (double) cnt / req_tot;
    if(freq > MIN_FREQ) {
	    cfl_add(ip_ntos(*(u_int32_t*)addr->value), freq, &cfl);
	}
}

void Client::onRequestReceived(http_pair_t *pair, flow_t *flow) {
	cout << "request received " << endl;
	
	req_tot++;

	request_t *req = pair->request_header;
	u_int32_t* addr_ = CALLOC(u_int32_t, 1);
	*addr_ = flow->socket.saddr;
	Addr2* addr = new Addr2(addr_);
	
	ht->add(addr);
}

void Client::onIntervalExpired() {
	cfl_init(&cfl);
	extract_freq_ht();

	print();
	cfl_delete(&cfl);
}

void Client::extract_freq_ht() {
	for(int i=0; i<ht->getSize(); i++){
		pthread_mutex_lock(&(ht->buckets[i].mutex));
	}

	for(int i=0; i<ht->getSize(); i++){
		Node* n = ht->buckets[i].first;
		while(n != NULL) {
			Addr2* a = (Addr2*)(n->value);
			cflAdd(a, n->cnt);
			n = n->next;
		}
	}

	for(int i=0; i<ht->getSize(); i++){
		pthread_mutex_unlock(&(ht->buckets[i].mutex));
	}
}

void Client::onNewFlowReceived(flow_t *flow) {
}

void Client::onFlowUpdate(flow_t *flow) {
}

void Client::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void Client::onTimerExpired() {
}