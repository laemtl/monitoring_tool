#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h> /* getopt() */
#include <pcap.h>
#include <errno.h>
#include <signal.h> 
#include <inttypes.h>
#include <limits>

#include "raw_packet.h"
#include "flow.hpp"
#include "util.h"
#include "server.h"
#include "flowHashTable.hpp"
#include "timer.h"

int pak = 0;
int req_n = 0;
int rsp_n = 0;
int pak_deq = 0;

int raw_req = 0;
int raw_rsp = 0;
int flow_req = 0;
int flow_rsp = 0;

//int GP_CAP_FIN = 0; /* Flag for offline PCAP sniffing */

void
debugging_print(Analysis* analysis){
	while(1){
		if (analysis->status < 0) {
			break;
		} else {
			analysis->pq->print();
			analysis->fht->print();
			analysis->fq->print();
			sleep(1);
		}
	}
	pthread_exit(NULL);
}

/**
 * Help function to print usage information.
 */
void
print_usage(const char* pro_name){
	printf("Usage: %s -i interface [-f tracefile] [-p ipaddress]\n", pro_name);
}

/**
 * Parse packets' header information and return a packet_t object
 */
packet_t*
packet_preprocess(char *raw_data, const struct pcap_pkthdr *pkthdr)
{
	packet_t	*pkt = NULL;	/* new packet */
	char 	*cp = raw_data;
	ethhdr	*eth_hdr = NULL;
	iphdr	*ip_hdr = NULL;
	tcphdr	*tcp_hdr = NULL;

	/* Parse libpcap packet header */
	pkt = packet_new();
	pkt->cap_sec = pkthdr->ts.tv_sec;
	pkt->cap_usec = pkthdr->ts.tv_usec;
	pkt->raw_len = pkthdr->caplen;
	//sprintf(pkt->time_string,"%ld\n",pkt->cap_sec);
	//printf("time_string: %s\n",pkt->time_string);

	/* Parse ethernet header and check IP payload */
	eth_hdr = packet_parse_ethhdr(cp);
	if(eth_hdr->ether_type != 0x0800){
		free_ethhdr(eth_hdr);
		packet_free(pkt);
		return NULL;
	}

	/* Parse IP header and check TCP payload */
	cp = cp + sizeof(ethhdr);

	ip_hdr = packet_parse_iphdr(cp);
	pkt->saddr = ip_hdr->saddr;
	pkt->daddr = ip_hdr->daddr;

	/*char *saddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	char *daddr = malloc(sizeof("aaa.bbb.ccc.ddd"));
	strncpy(saddr, ip_ntos(pkt->saddr), sizeof("aaa.bbb.ccc.ddd"));
	strncpy(daddr, ip_ntos(pkt->daddr), sizeof("aaa.bbb.ccc.ddd"));
	printf("%s||%s\n",saddr,daddr);
	free(saddr);
	free(daddr);*/
	
	pkt->ip_hl = (ip_hdr->ihl) << 2;	/* bytes */
	pkt->ip_tol = ip_hdr->tot_len;
	pkt->ip_pro = ip_hdr->protocol;

	if(pkt->ip_pro != 0x06){
		free_ethhdr(eth_hdr);
		free_iphdr(ip_hdr);
		packet_free(pkt);
		return NULL;
	}

	/* Parse TCP header */
	cp = cp + pkt->ip_hl;
	tcp_hdr = packet_parse_tcphdr(cp);
	pkt->sport = tcp_hdr->th_sport;
	pkt->dport = tcp_hdr->th_dport;
	pkt->tcp_seq = tcp_hdr->th_seq;
	pkt->tcp_ack = tcp_hdr->th_ack;
	pkt->tcp_flags = tcp_hdr->th_flags;
	pkt->tcp_win = tcp_hdr->th_win;
	pkt->tcp_hl = tcp_hdr->th_off << 2;		/* bytes */
	pkt->tcp_dl = pkt->ip_tol - pkt->ip_hl - pkt->tcp_hl;
	pkt->http = 0; /* default */

	/* Check the TCP ports to identify if the packet carries HTTP data
	   We only consider normal HTTP traffic without encryption */
	if( !(tcp_hdr->th_sport == 80 || tcp_hdr->th_dport == 80 || \
		tcp_hdr->th_sport == 8080 || tcp_hdr->th_dport == 8080 || \
		tcp_hdr->th_sport == 8000 || tcp_hdr->th_dport == 8000)){

		// 11211
		
		free_ethhdr(eth_hdr);
		free_iphdr(ip_hdr);
		free_tcphdr(tcp_hdr);
		packet_free(pkt);
		return NULL;
	}

	/* Process packets of flows which carry HTTP traffic */
	if(pkt->tcp_dl != 0){
		cp = cp + pkt->tcp_hl;
		int i = 0;
		
		for (;i<pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(ethhdr));i++){
		//	printf("%c",cp[i]);
			//fflush(stdout);
		}
		if( !IsHttpPacket(cp, pkt->tcp_dl) ){
			/* If the packet is not HTTP, we erase the payload. */
			pkt->tcp_odata = NULL;
			pkt->tcp_data = pkt->tcp_odata;
			

			/** Added functionality: still keep chunked packets.
			 *  By default, if no header information, such as when packet is chunked
			 *  then the payload data is erased. The following commented code makes it so
			 *  the data is not erased. This is needed is using logger that
			 *  does its own size calculation.
			**/

		}else{
			/* Yes, it's HTTP packet */
			char *head_end = NULL;
			//int hdl = 0;
			head_end = IsRequest(cp, pkt->tcp_dl);
			
			if( head_end != NULL ){
				/* First packet of request. */
				req_n++;
				//hdl = head_end - cp + 1;
				pkt->http = HTTP_REQ;
		
				/** Added functionality: Real TCP data length
				 *  needed for logger that does own size calc
				 *  or for individual packet size
				**/
				pkt->tcp_dl = pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(ethhdr));
			}
			head_end = IsResponse(cp, pkt->tcp_dl);
			if( head_end != NULL ){
				/* First packet of response. */
				//hdl = head_end - cp + 1;
				pkt->http = HTTP_RSP;

				rsp_n++;
		
				pkt->tcp_dl=pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(ethhdr));
			}
			/* Allocate memory to store HTTP header. */
			pkt->tcp_odata = MALLOC(char, pkt->tcp_dl + 1);
			pkt->tcp_data = pkt->tcp_odata;
			memset(pkt->tcp_odata, 0, pkt->tcp_dl + 1);
			memcpy(pkt->tcp_odata, cp, pkt->tcp_dl);
                     
		}
	}else{
		pkt->tcp_odata = NULL;
		pkt->tcp_data = pkt->tcp_odata;
	}

	free_ethhdr(eth_hdr);
	free_iphdr(ip_hdr);
	free_tcphdr(tcp_hdr);
	return pkt;
}

void process_packet(Analysis* analysis) {
	packet_t *packet = NULL;
	raw_pkt *rpkt = NULL;
	
	while(1){
		rpkt = (raw_pkt*)analysis->rpq->deq();
		
		if (analysis->isStopped() && rpkt == NULL) {
			break;
		} else if (rpkt != NULL){
			packet = packet_preprocess(rpkt->raw, &(rpkt->pkthdr));
			raw_packet_free(rpkt);

			if (NULL != packet){
				analysis->pq->enq(packet);
			}
			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}
	pthread_exit("Packet raw processing finished.\n");
}

/**
 * Fetch a packet from packet queue and add it to any flow.
 */
int
process_packet_queue(Analysis* analysis){
	packet_t *pkt = NULL;
	
	while(1){
		pkt = analysis->pq->deq();
		if (analysis->isStopped()) {
			break;
		} else if (pkt != NULL){
			analysis->fht->add_packet(pkt);
			pak_deq++;
			
			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}
	pthread_exit("Packet processing finished.\n");
	return 0;
}

/**
 * Fetch a flow from flow queue and process it
 */
int
process_flow_queue(Analysis* analysis){
	Flow *flow = NULL;
	
	while(1){
		flow = analysis->fq->deq();

		if (analysis->status < 0) {
			break;
		} else if(flow != NULL){
			flow->extract_http(TRUE);
			analysis->extractData(flow);
			delete flow;
			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}
	pthread_exit("Flow processing finished.\n");
	return 0;
}

/**
 * Scrub flow hash table to forcely close dead flows.
 */
void
scrubbing_flow_htbl(Analysis* analysis){
	int num;

	while(1){
		sleep(10);
		if (analysis->isStopped()){
			num = analysis->fht->flow_scrubber(-1); // cleanse all flows
			printf("Cleaned: %d flows \n", num);
			break;
		} else {
			num = analysis->fht->flow_scrubber(60*10);	// flow timeout in seconds
			printf("Cleaned: %d flows \n", num);
		}
	}
	pthread_exit(NULL);
}

/**
 * Main capture function
 */
int
capture_main(void* a){
	char errbuf[PCAP_ERRBUF_SIZE];
	memset(errbuf, 0, PCAP_ERRBUF_SIZE);
	pcap_t *cap = NULL;
	
	Analysis* analysis = (Analysis*) a;
	const char* interface = analysis->interface;
	int livemode = analysis->livemode;

	const u_char *raw = NULL;
	struct pcap_pkthdr pkthdr;
	//packet_t *packet = NULL;
	//extern int GP_CAP_FIN;
	
	if ( livemode==1 ) {
		cap = pcap_open_live(interface, 65535, 0, 1000, errbuf);
	} else {
		cap = pcap_open_offline(interface, errbuf);
	}

	if( cap == NULL) {
		printf("errbuf %s\n", errbuf); 
		analysis->stop();
		pthread_exit(NULL);
	}

	raw_pkt* pkt2 = NULL;

	while(1){
		raw = pcap_next(cap, &(pkthdr));
		if(raw == NULL) continue;

		size_t len = pkthdr.len;
		// 60 is the minimum size for a valid ethernet packet size
		// 4 bytes CRC is stripped
		if(len < 54) continue;

		if ( livemode == 0 || analysis->status < 0) {
			//GP_CAP_FIN = 1;
			break;
		} else if( NULL != raw){
			pkt2 = MALLOC(raw_pkt, 1);
			pkt2->pkthdr = pkthdr;

			char* r = MALLOC(char, len);
			memcpy(r, raw, len);
			pkt2->raw = r;

			analysis->rpq->enq(pkt2);
			pak++;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}

	if( cap != NULL)
		pcap_close(cap);

	return 0;
}

void start_analysis(char* ipaddress, Analysis* analysis) {
	void *thread_result;
	time_t start, end;
	time(&start);

	pthread_t job_pkt;
	pthread_t job_pkt_q;
	pthread_t job_flow_q;
	pthread_t job_scrb_htbl;
	pthread_t timer;
	pthread_t capture;
#if DEBUGGING == 1
	pthread_t job_debug_p;
#endif

	analysis->livemode = 1;
	
	printf("Http-sniffer started: %s", ctime(&start));

	/* Start packet receiving thread */
	pthread_create(&job_pkt, NULL, (void*)process_packet, analysis);
	
	/* Start packet receiving thread */
	pthread_create(&job_pkt_q, NULL, (void*)process_packet_queue, analysis);
	
	/* Start dead flow cleansing thread */
	pthread_create(&job_scrb_htbl, NULL, (void*)scrubbing_flow_htbl, analysis);

	/* Start flow processing thread */
	pthread_create(&job_flow_q, NULL, (void*)process_flow_queue, analysis);

	pthread_create(&timer, NULL, (void*)start_timer, analysis);

#if DEBUGGING == 1
	pthread_create(&job_debug_p, NULL, (void*)debugging_print, analysis);
#endif

	/* Start main capture in live or offline mode */
	pthread_create(&capture, NULL, (void*)capture_main, analysis);

	// Wait for all threads to finish
	pthread_join(timer, NULL);
	pthread_join(capture, &thread_result);
	pthread_join(job_pkt_q, &thread_result);
	pthread_join(job_flow_q, &thread_result);
	pthread_join(job_scrb_htbl, &thread_result);
	pthread_join(job_pkt, &thread_result);

#if DEBUGGING == 1
	pthread_join(job_debug_p, &thread_result);
#endif
	
	time(&end);
	printf("Time elapsed: %d s\n", (int)(end - start));

	analysis->fht->clear();
	analysis->fht->reset();
}

void sigintHandler(int sig_num) { 
	signal(SIGINT, sigintHandler); 
    printf("pak: %d \n", pak);
	printf("dpak: %d \n", pak_deq);
	printf("reqn: %d \n", req_n);
	printf("rspn: %d \n", rsp_n);

	printf("raw_req: %d \n", raw_req);
	printf("raw_rsp: %d \n", raw_rsp);
	printf("flow_req: %d \n", flow_req);
	printf("flow_rsp: %d \n", flow_rsp);

	exit(0);
} 

/**
 * Main portal of http-sniffer
 */
int main(int argc, char *argv[]){
	char* interface = NULL;
	char* ipaddress = NULL;
	int opt;
	bool debug = false;

	signal(SIGINT, sigintHandler);
	printf("My process ID : %d\n", getpid());

   	// Parse arguments
	while((opt = getopt(argc, argv, ":i:f:o:p:h:d")) != -1){
		switch(opt){
			case 'h':
				print_usage(argv[0]); return (1);
			case 'i':
				interface = optarg; break;
			case 'p':
				ipaddress = optarg; break;
			case 'd':
				debug = true; break;
		}
	}
    
    // Interface is NULL - RUN in server mode	
	if (interface == NULL){
		/* Start server thread */
		pthread_t job_server;
		pthread_create(&job_server, NULL, (void*)start_server, &debug);
		pthread_join(job_server, NULL);
	
	// Interface is provided - RUN in app mode
	} else {		
		Analysis* analysis = new Analysis(-1, interface, 5, 600, false, debug);
		analysis->activeMetrics(std::numeric_limits<int>::max());
		start_analysis(ipaddress, analysis);
	}

	return 0;
}