#include "errRate.hpp"
#include "responseStatus.hpp"

ErrRate::ErrRate(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricAvg(protocol, analysis, "err_rate", "Error rate"){
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

void ErrRate::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
	// total number of received response
	subtotal->add(1);
	total->add(1);

	ResponseStatus *rsp = (ResponseStatus*)pair->response_header;				
	int status = rsp->statusCode;				
	if(rsp->hasErrorStatus()) {
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

void ErrRate::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
}