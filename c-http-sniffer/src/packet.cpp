#include "packet.hpp"

Packet::Packet() 
	: cap_sec(0), cap_usec(0), raw_len(0), saddr(0), daddr(0), sport(0), dport(0), ip_hl(0), 
	ip_tol(0), ip_pro(0), ip_pid(0), tcp_seq(0), tcp_ack(0), tcp_flags(0), tcp_win(0), tcp_hl(0), tcp_dl(0), http(0)	
{
	tcp_odata = NULL;
	tcp_data = tcp_odata;
	next = NULL;
};

/*
 * Free a memory block allocated to a packet_t object
 */
Packet::~Packet(){
	if(tcp_odata != NULL) {
		free(tcp_odata);
	}
}