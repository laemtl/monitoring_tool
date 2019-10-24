#include "metricAvg.hpp"

// inner var or extend ??
// maybe extend is better
class Rst: public MetricDouble, MetricAvg
{
	private:

	public:
		Rst();
		double getAvg();
		void onResponseReceived(http_pair_t *h, Flow *flow);
		void print();
}

Rst::Rst() {
	responseReceived.add(&this);
}

double Rst::getAvg() {
	int req_total = data->rst.getTotal(); 
	if (req_total != 0) return rst.getSum()/req_total;
	return 0;
}

void Rst::update(Event responseReceived, http_pair_t *h, Flow *flow) {
	if(!rst.isActive()) return;

	// Compute response time
	double rstV = (h->rsp_fb_sec + h->rsp_fb_usec * 0.000001) - (h->req_fb_sec + h->req_fb_usec * 0.000001);

	response_t *rsp = h->response_header;
	request_t *req = h->request_header;

	if(rst < 0 ) {
		printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
		printf("aknowledgment: %ld \n", rsp->acknowledgement);
	}
	
	addTotal(1);
	addSum(rstV);
	setMin(rstV);
	setMax(rstV);
}

void Rst::print() {
	if(isActive())
		printf("RST (avg, min, max): %f %f %f \n", result->rst_avg, result->rst_min, result->rst_max);
}