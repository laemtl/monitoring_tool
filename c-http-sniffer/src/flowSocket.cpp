#include "flowSocket.hpp"

/* Compare two flow_s objects */
int FlowSocket::compare(FlowSocket *fs){
	return memcmp(this, fs, sizeof(FlowSocket));
}

u_int32_t FlowSocket::hash(int size) {
	return ((sport & 0xff) | ((dport & 0xff) << 8) | ((saddr & 0xff) << 16) | ((daddr & 0xff) << 24)) % size;
}