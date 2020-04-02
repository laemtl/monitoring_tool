#include "metricCumDistr.hpp"

/*class MCDItem {
	public:
		char* name;
    	double freq;
};

// Vector ??
class MCDList {
	public:
		MCDItem list[CFL_SIZE];
		pthread_mutex_t mutex;
		int size;
		int count;

		MCDList();
		void add(char* name, double freq);
		void print();

};*/

/*MCDList::MCDList() 
	: size(CFL_SIZE), count(0)
{
    pthread_mutex_init(&mutex, NULL);
}

void MCDList::add(char* name, double freq) {
    pthread_mutex_lock(&mutex);
    if(count >= size) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    list[count].name = name;
    list[count].freq = freq;
    count++;
    pthread_mutex_unlock(&mutex);
}

void MCDList::print() {
    for(int i = 0; i < count; i++) {
        printf("name: %s \n", list[i].name);
        printf("freq: %f \n", list[i].freq);
    }
}*/

/*void MCDList::delete() {
    for(int i = 0; i < count; i++) {
        free(list[i].name);
    }
}*/

/*MetricCumDistr::~MetricCumDistr() {
}*/

MetricCumDistr::MetricCumDistr(_protocol::Protocol* protocol, Analysis* analysis, std::string name, std::string desc) 
: Metric(protocol, analysis, name, desc) {
	cfl_init(&cfl);
}

MetricCumDistr::~MetricCumDistr() {
	ht.clear();
	ht.reset();
}

void MetricCumDistr::subscribe(EventManager* em) {
	em->timerExpired->add(this);
	em->intervalExpired->add(this);
}

void MetricCumDistr::print() {
	if(analysis->debug) {
		std::cout << name << std::endl;

		std::cout << "count: " << cfl.count << std::endl;
		for(int i = 0; i < cfl.count; i++) {
			printf("name: %s \n", cfl.list[i].name);
			printf("freq: %f \n", cfl.list[i].c_freq);
		}

		//print_cfl(&(cfl));
		printf("\n");
	}
}

void MetricCumDistr::cflUpdate(Hash* ht) {
	for(int i=0; i<ht->getSize(); i++){
		pthread_mutex_lock(&(ht->buckets[i].mutex));
	}

	for(int i=0; i<ht->getSize(); i++){
		Node* n = ht->buckets[i].first;
		while(n != NULL) {
			Hashable* a = (Hashable*)(n->value);
			cflAdd(a, n->cnt);
			n = n->next;
		}
	}

	for(int i=0; i<ht->getSize(); i++){
		pthread_mutex_unlock(&(ht->buckets[i].mutex));
	}
}

void MetricCumDistr::sendMsg() {
	uint8_t *buf;              // Buffer to store serialized data
	uint64_t msg_len;          // Length of serialized data
	
	Analysis__MetricMsg msg = ANALYSIS__METRIC_MSG__INIT;
	msg.values_case = ANALYSIS__METRIC_MSG__VALUES_METRIC_CUM_DISTR;

	Analysis__MetricCumDistrMsg cumDistrMsg = ANALYSIS__METRIC_CUM_DISTR_MSG__INIT;
	msg.metriccumdistr = &cumDistrMsg;
	msg.name = (char*)name.c_str();
	msg.time = time(0);
	Analysis__Freq **freqs = MALLOC(Analysis__Freq*, cfl.count); 
	msg.metriccumdistr->freqs = freqs;
	msg.metriccumdistr->n_freqs = cfl.count;

	msg.netint = (char*)analysis->interface;

	for(int i = 0; i < cfl.count; i++) {
		Analysis__Freq* freq = MALLOC(Analysis__Freq, 1);
		*freq = ANALYSIS__FREQ__INIT;
		freq->name = cfl.list[i].name;
		freq->freq = cfl.list[i].c_freq;
		freqs[i] = freq;
	}

	msg_len = analysis__metric_msg__get_packed_size(&msg);
	
	// Max size of varint_len is 10 bytes
	buf = (uint8_t*)malloc(10 + msg_len);

	// Convert msg_len to a varint
	int varint_len_len = encode_varint(buf, msg_len);	
	
	analysis__metric_msg__pack(&msg, buf + varint_len_len);
	
	// send
	if(send(analysis->socket, buf, varint_len_len + msg_len, MSG_NOSIGNAL) < 0) {
		std::cout << "Error sending response for metric" << name << std::endl;
		perror("\nError is: ");
		error("Error sending response\n");
	} 

	free(buf); // Free the allocated serialized buffer
	for(int i = 0; i < cfl.count; i++) free(freqs[i]);
	free(freqs);	
}