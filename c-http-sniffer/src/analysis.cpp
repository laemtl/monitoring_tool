#include "analysis.hpp"

//template <class Num>
Analysis::Analysis(bool debug, const char* interface, uint32_t interval, char* ipaddress) 
    : status(1), debug(debug), clientSock(-1), interface(interface), interval(interval), ipaddress(ipaddress)
{
    eventManager = new EventManager();

    rawPktQueue = new Queue<RawPacket>();
    pktQueue = new PacketQueue();
    flowQueue = new FlowQueue();
    flowHashTable = new FlowHashTable(flowQueue);	

    start();
}

Analysis::Analysis(bool debug, int client_sock)
	: status(1), debug(debug), clientSock(client_sock) 
{
    eventManager = new EventManager();

    rawPktQueue = new Queue<RawPacket>();
    pktQueue = new PacketQueue();
    flowQueue = new FlowQueue();
    flowHashTable = new FlowHashTable(flowQueue);	

    //start();

}

bool Analysis::isServerMode() {
	return serverMode;
}

/*void Analysis::newFlowReceived(Flow *flow) {
    for (Observer *observer : observers) {
        observer->onNewFlowReceived(flow);
    }
}*/

void Analysis::start() {
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

	//param->fd = ipaddress;
	livemode = 1;
	
	printf("Http-sniffer started: %s", ctime(&start));

	/* Initialization of packet and flow data structures */
	
	//packet_queue_init((void*)analysis);
	//analysis.status = 1;


	// TODO : does the function received its context (Analysis)??

	/* Start packet receiving thread */
	pthread_create(&job_pkt, NULL, (THREADFUNCPTR) &processRawPktQueue, NULL);
	
	/* Start packet receiving thread */
	pthread_create(&job_pkt_q, NULL, (THREADFUNCPTR) &processPacketQueue, NULL);
	
	/* Start dead flow cleansing thread */
	pthread_create(&job_scrb_htbl, NULL, (THREADFUNCPTR) &scrubbingFlowHashTable, NULL);

	/* Start flow processing thread */
	pthread_create(&job_flow_q, NULL, (THREADFUNCPTR) &processFlowQueue, NULL);

	pthread_create(&timer, NULL, (THREADFUNCPTR) &start_timer, NULL);

#if DEBUGGING == 1
	pthread_create(&job_debug_p, NULL, (void*)debugging_print, data);
#endif

	/* Start main capture in live or offline mode */
	pthread_create(&capture, NULL, (THREADFUNCPTR) &capture_main, NULL);

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
}

void Analysis::debugging_print(){
	while(1){
		if (status < 0) {
			break;
		} else {
			pktQueue->print();
			flowHashTable->print();
			flowQueue->print();
			sleep(1);
		}
	}
	pthread_exit(NULL);
}

/**
 * Fetch a flow from flow queue and process it
 */
void Analysis::processFlowQueue(){
	Flow *flow = NULL;
	//thread_init(data);

	while(1){
		flow = flowQueue->deq();

		if (status < 0) {
			break;
		} else if(flow != NULL){
			flow->extract_http(true);

		    // move on flow
			extract_data(flow);
			delete flow;
			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}

	// Flow processing finished
	pthread_exit(NULL);
}

// TODO : where ahould this be placed?? FlowHashTable??
void Analysis::processFlowHashTable() {
	Flow *flow, *flow_next = NULL;

	for(int i=0; i<flowHashTable->getSize(); i++){
		pthread_mutex_lock(&(flowHashTable->flowHashTable[i]->mutex));
		flow = flowHashTable->flowHashTable[i]->first;
		
		while(flow != NULL ){
			flow_next = flow->next;
			flow->extract_http(false);
			extract_data(flow);
			flow = flow_next;
		}
		
		pthread_mutex_unlock(&flowHashTable->flowHashTable[i]->mutex);
	}
}


void Analysis::extract_data(Flow *flow){	
	if(has_client_ip) {
		if(client.sin_addr.s_addr != flow->socket.saddr) return;
	}

	if(has_client_port) {
		if(client.sin_port != flow->socket.sport) return;
	}

	if(has_server_ip) {
		if(server.sin_addr.s_addr != flow->socket.daddr) return;
	}

	if(has_server_port) {
		if(server.sin_port != flow->socket.dport) return;
	}

	if(!flow->processed) {
		flow->processed = TRUE;	

		eventManager->newFlowReceived.notify(flow);			
	}

	// Need client/server value
	eventManager->flowUpdate.notify(flow);

	if (flow->http_f != NULL){        	
		http_pair_t *pair = flow->http_f;
		while(pair != NULL) {
			if(pair->request_header != NULL) {
				if(!pair->req_processed) {
					pair->req_processed = TRUE;
					flow_req++;
					
					eventManager->requestReceived.notify(pair, flow);
				}
				
				// If response has been processed
				// We can discard the pair
				if(pair->rsp_processed) {
					http_pair_t* next = pair->next;

					if(next != NULL) {
						flow->http_f = next;
						http_free(pair);
					}
					pair = next;
					continue;
				}

				if(pair->response_header != NULL) {
					pair->rsp_processed = TRUE;
					flow_rsp++;
    
					//event::emit("responseReceived");
					eventManager->responseReceived.notify(pair, flow);
				}
				//if(h->response_header == NULL) d++;
			}	   
			pair = pair->next;
		}
	}
}

/**
 * Fetch a packet from packet queue and add it to any flow.
 */
void Analysis::processPacketQueue(){
	Packet *pkt = NULL;
	
	while(1){
		pkt = pktQueue->deq();
		if (status < 0) {
			break;
		} else if (pkt != NULL){
			flowHashTable->add_packet(pkt);
			pak_deq++;

			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}
	// Packet processing finished
	pthread_exit(NULL);
}

//TODO:  pass status +  flowhashtable
// Result* result
void Analysis::printData() {
/*	Data* data = {0};
	get_data(&data);*/

	time_t raw_time;
	struct tm *timeinfo = NULL;
	char time_buf[20];
	
	// Get local time
	time( &raw_time );
	timeinfo = localtime( &raw_time );
	memset(time_buf, 0, sizeof(time_buf));
	strftime(time_buf, sizeof(time_buf), "%Y%m%d %H:%M:%S", timeinfo);

/*	printf("Time: %s \n", time_buf);*/
}

// Called on timer expiration
void Analysis::processData(int sig) {
	//data->int_step++;

	// The completed flow are processed by extract_data every seconds
	// We process the ones in the hash table using the following function 	
    processFlowHashTable();

    eventManager->timerExpired.notify();
}

void Analysis::printTid() {
	pid_t x = syscall(__NR_gettid);
	printf("pid: %ld \n", (long)x);
}

/**
 * Main capture function
 */
void Analysis::capture_main(){
	char errbuf[PCAP_ERRBUF_SIZE];
	memset(errbuf, 0, PCAP_ERRBUF_SIZE);
	pcap_t *cap = NULL;
	
	//const char* interface = analysis.interface;
	//int livemode = param->livemode;
	
	//thread_init(data);
	//rawPktQueue = new Queue();

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
		status = -1;
		pthread_exit(NULL);
	}

	RawPacket* pkt2 = NULL;

	while(1){
		raw = pcap_next(cap, &(pkthdr));
		
		if ( livemode == 0 || status < 0) {
			//GP_CAP_FIN = 1;
			break;
		} else if( NULL != raw){
			pkt2 = new RawPacket();
			pkt2->pkthdr = pkthdr;
			size_t len = pkthdr.len;

			char* r = MALLOC(char, len);
        	memcpy(r, raw, len);
			pkt2->raw = r;
			
			rawPktQueue->enq(pkt2);
			pak++;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}

	if( cap != NULL)
		pcap_close(cap);
}

void Analysis::stop(int sig) {
	status = -1;

	//timer_delete(tid);
	pthread_exit(NULL);
}

/**
 * Scrub flow hash table to forcely close dead flows.
 */
void Analysis::scrubbingFlowHashTable(){
	int num;
	
	while(1){
		sleep(10);
		if (status == 1){
			num = flowHashTable->flow_scrubber(60*10);	// flow timeout in seconds
			printf("Cleaned: %d flows \n", num);
		} else if (status == -1){
			num = flowHashTable->flow_scrubber(-1); // cleanse all flows
			printf("Cleaned: %d flows \n", num);
			break;
		}
	}
	pthread_exit(NULL);
}

/**
 * Parse packets' header information and return a packet_t object
 */
Packet* Analysis::packetPreprocess(char *raw_data, const struct pcap_pkthdr *pkthdr)
{
	Packet	*pkt = NULL;	/* new packet */
	char 	*cp = raw_data;
	EthHeader	*eth_hdr = NULL;
	IpHeader	*ip_hdr = NULL;
	TcpHeader	*tcp_hdr = NULL;

	/* Parse libpcap packet header */
	pkt = new Packet();
	pkt->cap_sec = pkthdr->ts.tv_sec;
	pkt->cap_usec = pkthdr->ts.tv_usec;
	pkt->raw_len = pkthdr->caplen;
	//sprintf(pkt->time_string,"%ld\n",pkt->cap_sec);
	//printf("time_string: %s\n",pkt->time_string);

	/* Parse ethernet header and check IP payload */
	eth_hdr = new EthHeader(cp);
	if(eth_hdr->ether_type != 0x0800){
		delete eth_hdr;
		delete pkt;
		return NULL;
	}

	/* Parse IP header and check TCP payload */
	cp = cp + sizeof(EthHeader);
	ip_hdr = new IpHeader(cp);
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
		delete eth_hdr;
		delete ip_hdr;
		delete pkt;
		return NULL;
	}

	/* Parse TCP header */
	cp = cp + pkt->ip_hl;
	tcp_hdr = new TcpHeader(cp);
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
		delete eth_hdr;
		delete ip_hdr;
		delete tcp_hdr;
		delete pkt;
		return NULL;
	}

	/* Process packets of flows which carry HTTP traffic */
	if(pkt->tcp_dl != 0){
		cp = cp + pkt->tcp_hl;
		int i = 0;
		
		for (;i<pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(EthHeader));i++){
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
				pkt->http = Packet::HTTP_REQ;
		
				/** Added functionality: Real TCP data length
				 *  needed for logger that does own size calc
				 *  or for individual packet size
				**/
				pkt->tcp_dl = pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(EthHeader));
			}
			head_end = IsResponse(cp, pkt->tcp_dl);
			if( head_end != NULL ){
				/* First packet of response. */
				//hdl = head_end - cp + 1;
				pkt->http = Packet::HTTP_RSP;

				rsp_n++;
		
				pkt->tcp_dl=pkt->raw_len-(pkt->ip_hl+pkt->tcp_hl+sizeof(EthHeader));
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
	delete(eth_hdr);
	delete(ip_hdr);
	operator delete(tcp_hdr);
	return pkt;
}

void Analysis::processRawPktQueue() {
	//thread_init(data);

	Packet *packet = NULL;
	RawPacket *rpkt = NULL;
	
	while(1){
		rpkt = rawPktQueue->deq();
		
		if (status < 0 && rpkt == NULL) {
			break;
		} else if (rpkt != NULL){
			packet = packetPreprocess(rpkt->raw, &(rpkt->pkthdr));
			delete rpkt;

			if (NULL != packet){
				pktQueue->enq(packet);
			}
			continue;
		} else {
			nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
		}
	}

	// Packet raw processing finished
	pthread_exit(NULL);
}