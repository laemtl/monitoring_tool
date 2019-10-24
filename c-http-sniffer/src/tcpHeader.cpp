#include "tcpHeader.hpp"

/*
 * Parse the TCP header with little endian format
 */

TcpHeader::TcpHeader(const char *p)
	: th_sport(0), th_dport(0), th_seq(0), th_ack(), th_x2(0), th_off(0), th_flags(0), th_win(0), th_sum(0), th_urp(0)
{
	TcpHeader *tmp;
	tmp = (TcpHeader *)p;

	th_sport = ntohs(tmp->th_sport);
	th_dport = ntohs(tmp->th_dport);
	th_seq = ntohl(tmp->th_seq);
	th_ack = ntohl(tmp->th_ack);
	th_x2 = tmp->th_x2;
	th_off = tmp->th_off;
	th_flags = tmp->th_flags;
	th_win = ntohs(tmp->th_win);
	th_sum = ntohs(tmp->th_sum);
	th_urp = ntohs(tmp->th_urp);
}