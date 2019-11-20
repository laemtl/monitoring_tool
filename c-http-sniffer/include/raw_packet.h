#ifndef __RAW_PKT_H__
#define __RAW_PKT_H__

#include <pcap.h>

#define	RAW_PKT_SIZE 100

typedef struct _raw_pkt raw_pkt;
struct _raw_pkt {
	char *raw;
	struct pcap_pkthdr pkthdr;
};

void raw_packet_free(raw_pkt* p) {
    free(p->raw);
    free(p);
}
#endif