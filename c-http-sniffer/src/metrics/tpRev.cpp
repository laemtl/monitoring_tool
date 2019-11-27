
#include "tpRev.hpp"

TpRev::TpRev(Protocol* protocol, Analysis* analysis)
: MetricAvg(protocol, analysis, "tp_rev", "Reverse Throughput"), timerRef(0), intervalRef(0) {
}

void TpRev::onFlowUpdate(Flow* flow) {
	u_int32_t payload = flow->payload_dst;
	total->set(payload);
}

double TpRev::getAvg() {
	if(analysis->interval <= 0) return 0;
	return (total->get() - intervalRef) / analysis->interval;
}

void TpRev::subscribe(EventManager* em) {
	em->flowUpdate->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void TpRev::onNewFlowReceived(Flow* flow) {
}

void TpRev::onRequestReceived(Pair *pair, Flow* flow) {
}

void TpRev::onResponseReceived(Pair *pair, Flow* flow) {
}

void TpRev::onTimerExpired() {
	MetricAvg::onTimerExpired();

	if(total->get() > 0) {
		u_int32_t newTimerRef = total->get();
		u_int32_t subtotal = newTimerRef - timerRef;
		timerRef = newTimerRef;
		
		min->minimize(subtotal);
		max->maximize(subtotal);	
	}
}

void TpRev::reset() {
	intervalRef = total->get();
	MetricAvg::reset();
}