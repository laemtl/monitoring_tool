#include "metric.hpp"

class Rst: public MetricAvg<double>
{
	private:
        String name;
    public:
		double Rst();
		double getAvg();
		void compute(http_pair_t *h);
		void print();
}

double Rst::getAvg() {
	Data* data = {0};
	get_data(&data);

	int req_total = data->rst.getTotal(); 
	if (req_total != 0) return rst.getSum()/req_total;
	return 0;
}

void Rst::compute(http_pair_t *h) {
	if(!rst.isActive()) return;

	// Compute response time
	double rstV = (h->rsp_fb_sec + h->rsp_fb_usec * 0.000001) - (h->req_fb_sec + h->req_fb_usec * 0.000001);

	response_t *rsp = h->response_header;
	request_t *req = h->request_header;

	if(rst < 0 ) {
		printf("req nxt seq: %" PRIu32 "\n", req->nxt_seq);
		printf("aknowledgment: %ld \n", rsp->acknowledgement);
	}
	
	rst.addTotal(1);
	rst.addSum(rstV);
	rst.setMin(rstV);
	rst.setMax(rstV);
}

double Rst::print();

void onTimerExpired();

void onIntervalExpired();