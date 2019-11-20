#include "errRate.hpp"

ErrRate::ErrRate(Analysis* analysis) 
: MetricAvg(analysis, "err_rate", "Error rate"){
}

double ErrRate::getRate() {
	double result = 0;
	if (subtotal->get() <= 0) return 0;
	return result = subsum->get()/subtotal->get();
}

void ErrRate::subscribe(EventManager* em) {
	em->responseReceived->add(this);
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void ErrRate::onResponseReceived(pair_t *pair, Flow* flow) {
	// total number of received response
	subtotal->add(1);
	total->add(1);

	response_t *rsp = pair->response_header;					
	int status = rsp->status;
	
	// Extract first digit of status code
	int i = status;
	while (i>=10) i=i/10;  
	if (i>3) {
		subsum->add(1);
		sum->add(1);
	}
}

void ErrRate::onTimerExpired() {
	double rate = getRate();
	min->minimize(rate);
	max->maximize(rate);

	subsum->set(0);
	subtotal->set(0);
}

void ErrRate::onNewFlowReceived(Flow* flow) {
}

void ErrRate::onFlowUpdate(Flow* flow) {
}

void ErrRate::onRequestReceived(pair_t *pair, Flow* flow) {
}