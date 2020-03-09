#include "rst.hpp"
#include "http.hpp"

Rst::Rst(Protocol* protocol, Analysis* analysis) 
: MetricAvg(protocol, analysis, "rst", "Request service time") {
}

void Rst::subscribe(EventManager* em) {
	em->responseReceived->add(this);
	em->intervalExpired->add(this);
}

void Rst::onResponseReceived(Pair *pair, Flow* flow) {
	//if(!isActive()) return;

	// Compute response time
	double rst = (pair->rsp_fb_sec + pair->rsp_fb_usec * 0.000001) - (pair->req_fb_sec + pair->req_fb_usec * 0.000001);

	_http::Response *rsp = (_http::Response*)pair->response_header;
	_http::Request *req = (_http::Request*)pair->request_header;

	//if(rst < 0 ) {
	//	printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
	//	printf("aknowledgment: %ld \n", rsp->acknowledgement);
	//}
	
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

void Rst::onRequestReceived(Pair *pair, Flow* flow) {

}