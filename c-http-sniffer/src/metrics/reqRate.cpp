// total -> sum

#include "reqRate.hpp"

ReqRate::ReqRate() {
	name = "REQ RATE NEW";
}

void ReqRate::subscribe(EventManager* em) {
	em->requestReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void ReqRate::onRequestReceived(http_pair_t *pair, flow_t *flow) {
	subtotal->add(1);
}

void ReqRate::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void ReqRate::onTimerExpired() {
	int req_subtotal = subtotal->get();
	
	min->minimize(req_subtotal);
	max->maximize(req_subtotal);
	sum->add(req_subtotal);
	subtotal->set(0);

	// total / interval value
	total->add(1);
}

void ReqRate::onNewFlowReceived(flow_t *flow) {
}

void ReqRate::onFlowUpdate(flow_t *flow) {
}