// total -> sum

#include "reqRate.hpp"

ReqRate::ReqRate(Analysis* analysis) 
: MetricAvg(analysis, "req_rate", "Request rate") {
}

void ReqRate::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

double ReqRate::getAvg() {
	if (analysis->interval <= 0) return 0;
	return sum->get()/analysis->interval;
}

void ReqRate::onRequestReceived(pair_t *pair, Flow* flow) {
	subtotal->add(1);
}

void ReqRate::onResponseReceived(pair_t *pair, Flow* flow) {
}

void ReqRate::onTimerExpired() {
	int req_subtotal = subtotal->get();
	
	min->minimize(req_subtotal);
	max->maximize(req_subtotal);
	sum->add(req_subtotal);
	subtotal->set(0);
}

void ReqRate::onNewFlowReceived(Flow* flow) {
}

void ReqRate::onFlowUpdate(Flow* flow) {
}