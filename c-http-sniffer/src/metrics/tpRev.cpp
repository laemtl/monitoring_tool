
#include "tpRev.hpp"

TpRev::TpRev(Analysis* analysis)
: MetricAvg(analysis, "tp_rev", "Reverse Throughput"), timerRef(0), intervalRef(0) {
}

void TpRev::onFlowUpdate(flow_t *flow) {
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

void TpRev::onNewFlowReceived(flow_t *flow) {
}

void TpRev::onRequestReceived(http_pair_t *pair, flow_t *flow) {
}

void TpRev::onResponseReceived(http_pair_t *pair, flow_t *flow) {
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