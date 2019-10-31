// TODO: separate protobuf
#include "metricAvg.hpp"

MetricAvg::MetricAvg() {
	subtotal = new Value();
    total = new Value();
    subsum = new Value();
	sum = new Value();

	avg = new Value();
    min = new Value();
    max = new Value();

	interval = 0;

    reset();
}

double MetricAvg::getAvg() {
	if (total->isClose(0)) return 0;
	return sum->get() / total->get();
}

void MetricAvg::print() {
	cout << name << " (avg, min, max): " << avg->get() << " " << min->get() << " " << max->get() << endl;
}

void MetricAvg::reset() {
	total->set(0);
	subtotal->set(0);
	sum->set(0);
	subsum->set(0);

	avg->set(0);
	min->set(numeric_limits<double>::max());
	max->set(0);

	interval = 0;
}

void MetricAvg::onTimerExpired() {
	interval++;
}

void MetricAvg::onIntervalExpired() {
	avg->set(getAvg());
	double minVal = min->get();
    double maxVal = max->get();

	print();
	reset();

	//if(!isActive()) return;
	
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