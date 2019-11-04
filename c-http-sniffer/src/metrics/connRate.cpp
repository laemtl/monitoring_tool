#include "connRate.hpp"

ConnRate::ConnRate(Analysis* analysis) 
: MetricAvg(analysis, "conn_rate", "Connection rate") {
}

void ConnRate::subscribe(EventManager* em) {
	em->newFlowReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void ConnRate::onNewFlowReceived(flow_t *flow) { 	
	subtotal->add(1);
}

double ConnRate::getAvg() {
	if(analysis->interval <= 0) return 0;
	return sum->get()/analysis->interval;
}

void ConnRate::onTimerExpired() {
	int connCnt = subtotal->get();
	sum->add(connCnt);
	min->minimize(connCnt);
	max->maximize(connCnt);
	subtotal->set(0);

	MetricAvg::onTimerExpired();
}

void ConnRate::onFlowUpdate(flow_t *flow) {
}

void ConnRate::onRequestReceived(http_pair_t *pair, flow_t *flow) {
}

void ConnRate::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}