#include "seq.hpp"

Seq::Seq(Packet *p)
	: pkt(NULL), seq(0), nxt_seq(0), ack(false), th_flags(0), cap_sec(0), cap_usec(0), processed(false), size(0)
{
	pkt = p;

	if ((p->tcp_flags & TcpHeader::TH_ACK) == TcpHeader::TH_ACK)
		ack = true;
	else
		ack = false;

	next = NULL;
	seq = p->tcp_seq;
	if((p->tcp_flags & TcpHeader::TH_SYN) == TcpHeader::TH_SYN || (p->tcp_flags & TcpHeader::TH_FIN) == TcpHeader::TH_FIN){
		nxt_seq = p->tcp_seq + p->tcp_dl +1;
	}else{
		nxt_seq = p->tcp_seq + p->tcp_dl;
	}
	th_flags = p->tcp_flags;
	cap_sec = p->cap_sec;
	cap_usec = p->cap_usec;

	size = p->tcp_dl;
}


int Seq::compare(Seq *seq){
	u_int32_t	sec = seq->cap_sec;
	u_int32_t	usec = seq->cap_usec;

	int ret = 0;

	if(cap_sec > sec || (cap_sec == sec && cap_usec > usec)){
		ret = 1;
	}else if (cap_sec < sec || (cap_sec == sec && cap_usec < usec)){
		ret = -1;
	}else{
		ret = 0;
	}

	return ret;
}
