#include "connRate.hpp"

ConnRate::ConnRate() {
	name = "CONN RATE NEW";
}

void ConnRate::subscribe(EventManager* em) {
	em->newFlowReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void ConnRate::onNewFlowReceived(flow_t *flow) { 	
	printf("new flow \n");
	subtotal->add(1);
}

// TODO remove conn_int_cnt from hashtable
void ConnRate::onTimerExpired() {
	int connCnt = subtotal->get();
	sum->add(connCnt);
	min->minimize(connCnt);
	max->maximize(connCnt);
	subtotal->set(0);

	// increment interval cnt
	total->add(1);

	MetricAvg::onTimerExpired();
}

void ConnRate::onFlowUpdate(flow_t *flow) {
}

void ConnRate::onRequestReceived(http_pair_t *pair, flow_t *flow) {
}

void ConnRate::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}