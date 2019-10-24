rsp_sec_tot;
rsp_int_tot;

double getAvg() {
	Data* data = {0};
	get_data(&data);

	double result = 0;
	if (rsp_int_tot > 0) {
		result = (double)data->err_rate.getTotal()/rsp_int_tot;
	}
	return result;
}

// Get error rate with subtotal
double get_err_rate_subtotal() {
	Data* data = {0};
	get_data(&data);

	double result = 0;
	if (data->rsp_sec_tot > 0) {
		result = (double)data->err_rate.getSubtotal()/data->rsp_sec_tot;
	}
	return result;
}

void reset() {
	parent.reset();
	rsp_int_tot = 0;
}

void Rst::onTimerExpires() {
	// compute rate

	rsp_sec_tot = 0;
}

void onResponseReceived(http_pair_t *h, , Flow *flow) {
	if(!isActive()) return;
	
	rsp_sec_tot++;
	rsp_int_tot++;
	
	response_t *rsp = h->response_header;					
	int status = rsp->status;
	
	// Extract first digit of status code
	int i = status;
	while (i>=10) i=i/10;  
	if (i>3) addSubtotal(1);
}

void ErrRate::print() {
	if(isActive())
		printf("ERR RATE (avg, min, max): %f %f %f \n", result->err_rate, result->err_rate_min, result->err_rate_max);
}

void Rst::onTimerExpires() {
	if(isActive()) {
		double err_rate_subtotal = get_err_rate_subtotal();
		setMin(err_rate_subtotal);
		setMax(err_rate_subtotal);

		// get and reset
		addTotal(getSubtotal());
		resetSubtotal();
	}
}
