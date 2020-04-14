#include "rst.hpp"

Rst::Rst(_protocol::Protocol* protocol, Analysis* analysis) 
: MetricAvg(protocol, analysis, "rst", "Request service time") {
}

void Rst::subscribe(EventManager* em) {
	em->responseReceived->add(this);
	em->intervalExpired->add(this);
}

void Rst::onResponseReceived(_protocol::Pair *pair, Flow* flow) {
	
	printf("Response received \n");

	// Compute response time
	double rst = (pair->rsp_fb_sec + pair->rsp_fb_usec * 0.000001) - (pair->req_fb_sec + pair->req_fb_usec * 0.000001);	
	total->add(1);
	sum->add(rst);
	min->minimize(rst);
	max->maximize(rst);
}

void Rst::onTimerExpired() {
}

void Rst::onNewFlowReceived(Flow* flow) {
}

void Rst::onFlowUpdate(Flow* flow) {
}

void Rst::onRequestReceived(_protocol::Pair *pair, Flow* flow) {
}