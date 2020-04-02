#include "analysis.hpp"

Analysis::Analysis(int socket, const char* interface, uint32_t interval, uint32_t duration) 
: interface(interface), socket(socket), interval(interval), duration(duration), serverMode(true), int_step(0), debug(false), status(0), livemode(1) {
	rpq = new Queue();
}

bool Analysis::isServerMode() {
    return serverMode;
}

void Analysis::stop() {
	status = -1;

	if(debug) {
		//printf("pak: %d \n", pak);
		//printf("dpak: %d \n", pak_deq);
		//printf("reqn: %d \n", req_n);
		//printf("rspn: %d \n", rsp_n);
	}

	// TODO 
	// loop on registered events
}

bool Analysis::isStopped() {
	return (status == -1);
}