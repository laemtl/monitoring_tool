#include "tp.hpp"

Tp::Tp(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricAvg(protocol, analysis, "tp", "Throughput"), timerRef(0), intervalRef(0) {
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

void Tp::onNewFlowReceived(Flow* flow) {
}

void Tp::onFlowUpdate(Flow* flow) {
	// TODO Check client / server value
	u_int32_t payload = flow->payload_src;
	total->set(payload);
}

void Tp::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
}

void Tp::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
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