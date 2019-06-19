#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h> /* getopt() */
#include <pcap.h>
#include <errno.h>

#include "flow.h"
#include "util.h"
#include "server.h"

// TODO: Be sure to lock before editing global variable
//int GP_CAP_FIN = 0; /* Flag for offline PCAP sniffing */

void
debugging_print(Data* data){
	thread_init(data);

	while(1){
		if ( !data->running ) {
			break;
		} else {
			packet_queue_print();
			flow_hash_print();
			flow_queue_print();
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
packet_preprocess(const char *raw_data, const struct pcap_pkthdr *pkthdr)
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
	/* char sport[12];
	char dport[12];
	setbuf(stdout,NULL);
	sprintf(sport,"%d",pkt->sport);
	sprintf(dport,"%d",pkt->dport);
	printf("%s||%s\n",sport,dport);*/
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

			/*pkt->tcp_dl=pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(ethhdr));
			pkt->tcp_odata = MALLOC(char, pkt->tcp_dl + 1);
			pkt->tcp_data = pkt->tcp_odata;
			memset(pkt->tcp_odata, 0, pkt->tcp_dl + 1);
			memcpy(pkt->tcp_odata, cp, pkt->tcp_dl);*/

		}else{
			/* Yes, it's HTTP packet */
			char *head_end = NULL;
			int hdl = 0;
			head_end = IsRequest(cp, pkt->tcp_dl);
			if( head_end != NULL ){
				/* First packet of request. */
				//total++;
				//printf("%d\n",total);
				hdl = head_end - cp + 1;
				pkt->http = HTTP_REQ;
				/* Fake TCP data length with only HTTP header. */
				//pkt->tcp_dl = hdl;

				/** Added functionality: Real TCP data length
				 *  needed for logger that does own size calc
				 *  or for individual packet size
				**/
				pkt->tcp_dl = pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(ethhdr));
			}
			head_end = IsResponse(cp, pkt->tcp_dl);
			if( head_end != NULL ){
				/* First packet of response. */
				hdl = head_end - cp + 1;
				pkt->http = HTTP_RSP;
				/* Fake TCP data length with only HTTP header. */
				//pkt->tcp_dl = hdl;
				
				/** Added functionality: Real TCP data length
				 *  needed for logger that does own size calc
				 *  or for individual packet size
				**/

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

/**
 * Fetch a packet from packet queue and add it to any flow.
 */
int
process_packet_queue(Data* data){
	packet_t *pkt = NULL;
	thread_init(data);
	
	while(1){
		// TODO: check how to handle the offline mode
		// Change was made
		pkt = packet_queue_deq();
		if ( !data->running ) {
			break;
		} else if (pkt != NULL){
			flow_hash_add_packet(pkt);
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
process_flow_queue(Data* data){
	flow_t	*flow = NULL;
	thread_init(data);

	while(1){
		flow = flow_queue_deq();

		// TODO: check how to handle the offline mode
		// Change was made
		if (!data->running) {
			break;
		} else if(flow != NULL){
			flow_extract_http(flow);
			extract_data(flow);
			flow_free(flow);

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
scrubbing_flow_htbl(Data* data){
	int num = 0;
	thread_init(data);

	while(1){
		sleep(10);
		if (data->running){
			num = flow_scrubber(60*10);	/* flow timeout in seconds */
		} else {
			num = flow_scrubber(-1); /* cleanse all flows */
			break;
		}
	}
	pthread_exit(NULL);
}

/**
 * Main capture function
 */
int
capture_main(void* p){

	char errbuf[PCAP_ERRBUF_SIZE];
	memset(errbuf, 0, PCAP_ERRBUF_SIZE);
	char *raw = NULL;
	pcap_t *cap = NULL;
	struct pcap_pkthdr pkthdr;
	packet_t *packet = NULL;
	//extern int GP_CAP_FIN;

	Capture* param = (Capture*) p;
	Data* data = param->data;
	const char* interface = data->interface;
	int fd = param->fd;
	void (*pkt_handler)(void*) = param->pkt_handler;
	int livemode = param->livemode;
	
	thread_init(data);

	//TODO: offline mode doesn't exist anymore
	if ( livemode==1 ) {
		cap = pcap_open_live(interface, 65535, 0, 1000, errbuf);
	} else {
		cap = pcap_open_offline(interface, errbuf);
	}

	if( cap == NULL) {
		printf("errbuf %s\n", errbuf); 
		exit(1);
	}

	while(1){
		raw = pcap_next(cap, &pkthdr);
		if( NULL != raw && data->running){
			packet = packet_preprocess(raw, &pkthdr);
			if (NULL!= packet){
				pkt_handler(packet);

				//TODO: Check with this is commented! Memory leak.
				// Is it to solve the seg Fault?
				//packet_free(packet);
			}
		} else if ( livemode==0 || !data->running) {
			//GP_CAP_FIN = 1;
			break;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}

	if( cap != NULL)
		pcap_close(cap);

	return 0;
}

//TODO: check if sending is done to the correct client
void start_analysis(char* ipaddress, Data* data) {
	void *thread_result;
	time_t start, end;
	time(&start);

	pthread_t job_pkt_q;
	pthread_t job_flow_q;
	pthread_t job_scrb_htbl;
	pthread_t timer;
	pthread_t capture;
#if DEBUGGING == 1
	pthread_t job_debug_p;
#endif

	Capture* param = (Capture*)calloc(1, sizeof(Capture));
	param->fd = ipaddress;
	param->pkt_handler = packet_queue_enq;
	param->livemode = 1;
	param->data = data;

	printf("Http-sniffer started: %s", ctime(&start));

	/* Initialization of packet and flow data structures */
	
	packet_queue_init((void*) data);
	flow_init((void*) data);

	/* Start packet receiving thread */
	pthread_create(&job_pkt_q, NULL, (void*)process_packet_queue, data);
	
	/* Start dead flow cleansing thread */
	pthread_create(&job_scrb_htbl, NULL, (void*)scrubbing_flow_htbl, data);

	/* Start flow processing thread */
	pthread_create(&job_flow_q, NULL, (void*)process_flow_queue, data);

	// TODO, uniformize error handling	
	pthread_create(&timer, NULL, start_timer, data);

#if DEBUGGING == 1
	pthread_create(&job_debug_p, NULL, (void*)debugging_print, data);
#endif

	/* Start main capture in live or offline mode */
	//TODO: check if we need the offline mode
	pthread_create(&capture, NULL, (void*)capture_main, param);

	// Wait for all threads to finish
	pthread_join(timer, NULL);
	pthread_join(job_pkt_q, &thread_result);
	pthread_join(job_flow_q, &thread_result);
	pthread_join(job_scrb_htbl, &thread_result);
	pthread_join(capture, &thread_result);
#if DEBUGGING == 1
	pthread_join(job_debug_p, &thread_result);
#endif
	
	time(&end);
	printf("Time elapsed: %d s\n", (int)(end - start));
	free(data);
	free(param);
}

/**
 * Main portal of http-sniffer
 */
int main(int argc, char *argv[]){
	char* interface = NULL;
	//TODO: check if we need the offline mode

	char* ipaddress = NULL;
	int opt;

   	// Parse arguments
	while((opt = getopt(argc, argv, ":i:f:o:p:h")) != -1){
		switch(opt){
		case 'h':
			print_usage(argv[0]); return (1);
		case 'i':
			interface = optarg; break;
		case 'p':
			ipaddress = optarg; break;
		}
	}
    
    // Interface is NULL - RUN in server mode	
	if (interface == NULL){
		/* Start server thread */
		pthread_t job_server;
		pthread_create(&job_server, NULL, (void*)start_server, NULL);
		pthread_join(job_server, NULL);
	
	// Interface is provided - RUN in app mode
	} else {		
		// TODO: move this call in a thread (can be a botte neck in case of high-traffic)
		// Be sure this is thread specific
		Data* data = (Data*)calloc(1, sizeof(Data));
		if(data == NULL) return EXIT_FAILURE;		
		init_data(data);
		data->interface = interface;
		start_analysis(ipaddress, data);
	}

	return 0;
}
