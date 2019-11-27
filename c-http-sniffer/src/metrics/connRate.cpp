#include "connRate.hpp"

ConnRate::ConnRate(Protocol* protocol, Analysis* analysis) 
: MetricAvg(protocol, analysis, "conn_rate", "Connection rate") {
}

void ConnRate::subscribe(EventManager* em) {
	em->newFlowReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void ConnRate::onNewFlowReceived(Flow* flow) { 	
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

void ConnRate::onFlowUpdate(Flow* flow) {
}

void ConnRate::onRequestReceived(Pair *pair, Flow* flow) {
}

void ConnRate::onResponseReceived(Pair *pair, Flow* flow) {
}