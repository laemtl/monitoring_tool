#include "analysis2.hpp"

Analysis::Analysis(int socket, const char* interface, uint32_t interval) 
: interface(interface), socket(socket), interval(interval) {
    eventManager = new EventManager();
    metricManager = new MetricManager(this);
};

/* vector<bool> status */
void Analysis::activeMetrics(int activeMetrics) {

    /*if(status.size() != metricManager->metrics.size()) {
        cout << "Invalid status size";
        pthread_exit(NULL);
    }*/

    /*for (unsigned i = 0; i < status.size(); i++) {
        if(!status.at(i)) metricManager->metrics.erase(metricManager->metrics.begin() + i);
    }*/

	for (int i = metricManager->metrics.size() - 1; i >= 0; i--) {
		bool status = activeMetrics & (1<<i);
		
		if(!status) {
			metricManager->metrics.erase(metricManager->metrics.begin() + i);
		} else {
			// Register metric to its attached events
			metricManager->metrics.at(i)->subscribe(eventManager);
		}	
	}
}