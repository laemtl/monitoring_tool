#include "ethHeader.hpp"

/*
 * Parse the ethernet header with little endian format
 */
EthHeader::EthHeader(const char *p)
	: ether_dhost{0}, ether_shost{0}, ether_type(0)
{
	EthHeader *tmp = (EthHeader *)p;

	memcpy(ether_dhost, tmp->ether_dhost, 6 * sizeof(u_int8_t));
	memcpy(ether_shost, tmp->ether_shost, 6 * sizeof(u_int8_t));
	ether_type = ntohs(tmp->ether_type);
}
