#include "tp.hpp"

Tp::Tp(Analysis* analysis) 
: MetricAvg(analysis, "tp", "Throughput"), timerRef(0), intervalRef(0) {
}

double Tp::getAvg() {
	if(analysis->interval <= 0) return 0;
	return (total->get() - intervalRef) / analysis->interval;
}

void Tp::subscribe(EventManager* em) {
	em->flowUpdate->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void Tp::onNewFlowReceived(flow_t *flow) {
}

void Tp::onFlowUpdate(flow_t *flow) {
	u_int32_t payload = flow->payload_src;
	total->set(payload);
}

void Tp::onRequestReceived(http_pair_t *pair, flow_t *flow) {
}

void Tp::onResponseReceived(http_pair_t *pair, flow_t *flow) {
}

void Tp::onTimerExpired() {
	MetricAvg::onTimerExpired();

	if(total->get() > 0) {
		u_int32_t newTimerRef = total->get();
		u_int32_t subtotal = newTimerRef - timerRef;
		timerRef = newTimerRef;
		
		min->minimize(subtotal);
		max->maximize(subtotal);	
	}
}

void Tp::reset() {
	intervalRef = total->get();
	MetricAvg::reset();
}