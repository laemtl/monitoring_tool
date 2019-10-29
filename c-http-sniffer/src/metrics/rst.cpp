#include "rst.hpp"

/*double Rst::getAvg() {
	int req_total = total->get(); 
	if (req_total != 0) return sum->get()/req_total;
	return 0;
}*/

void Rst::subscribe(EventManager* em) {
	em->responseReceived->add(this);
}

void Rst::onResponseReceived(http_pair_t *pair, flow_t *flow) {
	//if(!isActive()) return;

	// Compute response time
	double rst = (pair->rsp_fb_sec + pair->rsp_fb_usec * 0.000001) - (pair->req_fb_sec + pair->req_fb_usec * 0.000001);

	response_t *rsp = pair->response_header;
	request_t *req = pair->request_header;

	if(rst < 0 ) {
		printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
		printf("aknowledgment: %ld \n", rsp->acknowledgement);
	}
	
	total->add(1);
	sum->add(rst);
	min->set(rst);
	max->set(rst);
}


void Rst::onTimerExpired() {

}

void Rst::onIntervalExpired() {

}

void Rst::onNewFlowReceived(flow_t *flow) {

}

void Rst::onFlowUpdate(flow_t *flow) {

}

void Rst::onRequestReceived(http_pair_t *pair, flow_t *flow) {

}
