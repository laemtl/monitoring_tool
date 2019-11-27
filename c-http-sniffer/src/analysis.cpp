#include "analysis.hpp"

Analysis::Analysis(int socket, const char* interface, uint32_t interval, uint32_t duration, bool serverMode, bool debug) 
: interface(interface), socket(socket), interval(interval), duration(duration), serverMode(serverMode), int_step(0), hasClientIp(false), hasServerIp(0), debug(debug), status(0) {
	rpq = new Queue();
}

bool Analysis::isServerMode() {
    return serverMode;
}

void Analysis::setClientIp(u_int32_t ip) {
    hasClientIp = true;
    client.ip = ip;				
}

void Analysis::setClientPort(u_int16_t port) {
    hasClientPort = true;
    client.port = port;	
}

void Analysis::setServerIp(u_int32_t ip) {
    hasServerIp = true;
    server.ip = ip;	
}

void Analysis::setServerPort(u_int16_t port) {
    hasServerPort = true;
    server.port = port;	
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