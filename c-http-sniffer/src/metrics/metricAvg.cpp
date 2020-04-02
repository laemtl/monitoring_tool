// TODO: separate protobuf
#include "metricAvg.hpp"
#include <inttypes.h>

MetricAvg::MetricAvg(_protocol::Protocol* protocol, Analysis* analysis, std::string name, std::string desc) 
: Metric(protocol, analysis, name, desc) {
	subtotal = new Value();
    total = new Value();
    subsum = new Value();
	sum = new Value();

	avg = new Value();
    min = new Value();
    max = new Value();

    reset();
}

void MetricAvg::subscribe(EventManager* em) {
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

double MetricAvg::getAvg() {
	if (total->isClose(0)) return 0;
	return sum->get() / total->get();
}

void MetricAvg::print() {
	if(analysis->debug)
		std::cout << name << " (avg, min, max): " << avg->get() << " " << min->get() << " " << max->get() << std::endl;
}

void MetricAvg::reset() {
	total->set(0);
	subtotal->set(0);
	sum->set(0);
	subsum->set(0);

	avg->set(0);
	min->set(std::numeric_limits<double>::max());
	max->set(0);
}

void MetricAvg::onTimerExpired() {
}

void MetricAvg::onIntervalExpired() {
	avg->set(getAvg());
	if(analysis->isServerMode()) sendMsg();
	print();
	reset();
}	

void MetricAvg::onAnalysisEnded() {
}

void MetricAvg::sendMsg() {
	uint8_t *buf;              // Buffer to store serialized data
	uint64_t msg_len;          // Length of serialized data
	
	Analysis__MetricMsg msg = ANALYSIS__METRIC_MSG__INIT;
	msg.values_case = ANALYSIS__METRIC_MSG__VALUES_METRIC_AVG;

	Analysis__MetricAvgMsg avgMsg = ANALYSIS__METRIC_AVG_MSG__INIT;
	msg.metricavg = &avgMsg;
	msg.name = (char*)name.c_str();
	msg.protocolname = protocol->name;
	msg.protocolid = 0;
	msg.netint = (char*)analysis->interface;
	msg.time = time(0);
	msg.metricavg->avg = avg->get();
	msg.metricavg->min = min->get();
	msg.metricavg->max = max->get();

	msg_len = analysis__metric_msg__get_packed_size(&msg);

	// Max size of varint_len is 10 bytes
	buf = CALLOC(uint8_t, 10 + msg_len);

	// Convert msg_len to a varint
	int varint_len_len = encode_varint(buf, msg_len);	
	analysis__metric_msg__pack(&msg, buf + varint_len_len);
	
	// send
	if(send(analysis->socket, buf, varint_len_len + msg_len, MSG_NOSIGNAL) < 0) {
		std::cout << "Error sending response for metric" << name << std::endl;
		error("Error sending response\n");
	} 

	free(buf); // Free the allocated serialized buffer
}