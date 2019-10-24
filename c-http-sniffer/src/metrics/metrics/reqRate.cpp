double getAvg() {
	int req_total = req_rate.getTotal();
	double result = 0;
	if (analysis.interval > 0) {
		result = (double)req_total/analysis.interval;
	}
	return result;
}

void ReqRate::print() {
	if(isActive())
		printf("REQ RATE (avg, min, max): %f %f %f \n", result->req_rate, result->req_rate_min, result->req_rate_max);
}

void ReqURI::onRequestReceived(http_pair_t *h, Flow *flow) {
	if(!isActive()) return;

	addSubtotal(1);
}

void Rst::onTimerExpires() {
	if(!isActive()) return;
	
	// sync pb
	// need to get and reset
	int req_subtotal = getSubtotal();
	setMin(req_subtotal);
	setMax(req_subtotal);
	addTotal(req_subtotal);
	resetSubtotal();
}