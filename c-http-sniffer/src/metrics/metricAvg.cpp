// TODO: separate protobuf
#include "metricAvg.hpp"

MetricAvg::MetricAvg() {
}

double MetricAvg::getAvg() {
	if (total != 0) return sum->get() / total->get();
}

void MetricAvg::print() {
	if(isActive())
		printf("%s (avg, min, max): %f %f %f \n", name, getAvg(), min->get(), max->get());
}

void MetricAvg::onIntervalExpired() {
	if(!isActive()) return;
	
	/*Result* result;

	result->interface = (char*)data->interface;	
	result->client_sock = data->client_sock;

	result->rst_avg = getAvg();
	result->rst_min = getMin();
    result->rst_max = getMax();

	// only for AvgMetric
	reset();

	if(debug) {
		print(result);
		fflush(stdout);
	}

	// Struct are passed by value so the code below will execute correctly in an MT env
	if(analysis.is_server_mode()) {
		send(result);
	} 

	//free_result(result);
	
	if(data->status < 0) {
		pthread_exit(NULL);		
	}*/
}	

void MetricAvg::prepareMsg(double avg, double min, double max) {
	uint8_t *buf;                 // Buffer to store serialized data
	uint64_t msg_len;          // Length of serialized data
		
	/*if(mValue->client_sock < 0) {
		error("Socket is not defined\n");
	}

	analysis::MetricValueMsg* msg = new analysis::MetricValueMsg();
	msg->set_name(mValue->name);
	msg->set_netint(mValue->interface);
	metric.set_avg(avg);
	metric.set_min(min);
	metric.set_max(max);*/
}