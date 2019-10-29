u_int32_t sec_ref_value;
u_int32_t int_ref_value;
	
double getAvg() {
	if(getTotal() <= 0) return 0;

	double result = (double)(getTotal() - int_ref_value)/analysis.interval;
	int_ref_value = getTotal();
	return result;
}

void TpRev::onFlowUpdate(flow_t *flow) {
	if(!isActive()) return;
	
	u_int32_t payload = flow->payload_dst;
	setTotal(payload);
}

void TpRev::onTimerExpires() {
	if(isActive()) {
		if(getTotal() > 0) {
			u_int32_t new_sec_ref_value = getTotal();
			u_int32_t tp_subtotal = new_sec_ref_value - sec_ref_value;
			sec_ref_value = new_sec_ref_value;
			
			setMin((int)tp_subtotal);
			setMax((int)tp_subtotal);	
		}
	}
}