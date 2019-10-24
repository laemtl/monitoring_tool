#ifndef __RAW_PKT_H__
#define __RAW_PKT_H__

#include <pcap.h>

#define	RAW_PKT_SIZE 100

class RawPacket 
{
	public:
		char *raw;
		struct pcap_pkthdr pkthdr;
		RawPacket();
		~RawPacket();
};

#endif