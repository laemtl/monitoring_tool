#include "ipHeader.hpp"

/*
 * Parse the IP header with little endian format
 */

IpHeader::IpHeader(const char *p)
	: ihl(0), version(0), tos(0), tot_len(0), id(0), frag_off(0), ttl(0), protocol(0), check(0), saddr(0), daddr(0)
{
	IpHeader *tmp;
	tmp = (IpHeader *)p;

	ihl = tmp->ihl;
	version = tmp->version;
	tos = tmp->tos;
	tot_len = ntohs(tmp->tot_len);
	id = ntohs(tmp->id);
	frag_off = ntohs(tmp->frag_off);
	ttl = tmp->ttl;
	protocol = tmp->protocol;
	check = ntohs(tmp->check);
	saddr = ntohl(tmp->saddr);
	daddr = ntohl(tmp->daddr);
}