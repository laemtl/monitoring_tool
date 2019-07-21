#ifndef __PACKET_RAW_H__
#define __PACKET_RAW_H__

#include <pcap.h>

#define	RAW_PKT_SIZE 100

typedef struct _raw_pkt raw_pkt;
struct _raw_pkt {
	char *raw;
	struct pcap_pkthdr pkthdr;
    pthread_mutex_t mutex;
};

#endif