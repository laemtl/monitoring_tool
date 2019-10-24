#include "flow.hpp"

Flow::Flow(FlowHashTable* fht, FlowQueue* fq)
	: fht(fht), fq(fq)
{
	order = new Order();
}

Flow::~Flow(){
	Packet *cp;
	http_pair_t	*h;
	
	while(pkt_dst_f != NULL){
		cp = pkt_dst_f;
		pkt_dst_f = pkt_dst_f->next;
		delete cp;
	}

	while(pkt_src_f != NULL){
		cp = pkt_src_f;
		pkt_src_f = pkt_src_f->next;
		delete cp;
	}

	if(order != NULL){
		delete order;
	}

	while(http_f != NULL){
		h = http_f;
		http_f = http_f->next;
		http_free(h);
	}
}

/* Reset a Flow object to reuse */
void Flow::reset(){
	Packet *cp;

	while(pkt_dst_f != NULL){
		cp = pkt_dst_f;
		pkt_dst_f = pkt_dst_f->next;
		delete cp;
	}

	while(pkt_src_f != NULL){
		cp = pkt_src_f;
		pkt_src_f = pkt_src_f->next;
		delete cp;
	}

	if(order != NULL){
		delete order;
		order = new Order();
	}
	/*
	 * Note: maintain the f->socket.
	 */
	pkt_src_e = NULL;
	pkt_dst_e = NULL;
	rtt = 0;
	syn_sec = 0;
	syn_usec = 0;
	ack2_sec = 0;
	ack2_usec = 0;
	fb_sec = 0;
	fb_usec = 0;
	lb_sec = 0;
	lb_usec = 0;
	last_action_sec = 0;
	last_action_usec = 0;
	payload_src = 0;
	payload_dst = 0;
	pkt_src_n = 0;
	pkt_dst_n = 0;
	pkts_src = 0;
	pkts_dst = 0;
	http = 0;
	close = 0;
}

/* Serve for flow_add_packet(). Add packet to the flow packet chain. */
int Flow::hook_packet(Packet *packet, bool src){
	/* set some pointers */
	Packet **pkt_f = NULL, **pkt_e = NULL;
	u_int32_t *pkt_num = NULL;

	if(src == TRUE){
		pkt_f = &pkt_src_f;
		pkt_e = &pkt_src_e;
		pkt_num = &pkt_src_n;
	}else{
		pkt_f = &pkt_dst_f;
		pkt_e = &pkt_dst_e;
		pkt_num = &pkt_dst_n;
	}

	if((*pkt_num) == 0){
		(*pkt_f) = packet;
	}else{
		(*pkt_e)->next = packet;
	}

	(*pkt_e) = packet;
	(*pkt_e)->next = NULL;
	(*pkt_num)++;
	return 0;
}

/* Serve for flow_add_packet(). 
   Update flow packet number and bytes, then update the last action time of flow. 
   Manage the http pair processing
*/
int Flow::cal_packet(Packet *packet, bool src){
	u_int32_t *pkt_num = NULL, *byt_num = NULL;
	struct timeval tv;
	struct timezone tz;
	Seq *seq = NULL;

	if(src == TRUE){
		pkt_num = &pkts_src;
		byt_num = &payload_src;
	}else{
		pkt_num = &pkts_dst;
		byt_num = &payload_dst;
	}

	(*pkt_num)++;
	(*byt_num) += packet->tcp_dl;

	/*
	 * Update the last action time of flow.
	 * Used to delete some dead flows forcedly.
	 */
	gettimeofday(&tv, &tz);
	last_action_sec = tv.tv_sec;
	last_action_usec = tv.tv_usec;
	
	/* Process ordering, which must be right AFTER flow info updated */
	seq = new Seq(packet);
	if ( packet->http == 0 ){
		/*
		 * The packet without http payload will NOT be stored.
		 */
		seq->pkt = NULL;
	}

	tcp_order(order, seq, src);
	//flow_extract_pairs(f, seq);

	return 0;
}




/* Add a http_pair_t object to flow's http_pair_t chain */
int Flow::add_http(http_pair_t *h){
	if(http_f == NULL){
		http_f = h;
	}else{
		http_e->next = h;
	}
	http_e = h;
	http_e->next = NULL;
	//f->http_cnt++;
	return 0;
}

/* Add a packet_t object to flow's packet_t chain */
int Flow::add_packet(Packet *packet, register bool src){
	pthread_mutex_lock(&(hmb->mutex));

	if( http == FALSE ){
		if( pkt_src_n >= 5){
			/* We make sure that the flow is not a HTTP flow,
			* then remove it */
			delete packet;

			delete fht->remove(this);
			pthread_mutex_unlock(&(hmb->mutex));
			return 1;
		}
	}

	/* TH_RST:
	* If the flow is reset by sender or receiver*/
	if((packet->tcp_flags & TcpHeader::TH_RST) == TcpHeader::TH_RST){
		if( pkts_src < 4){
			// Flow with uncomplete information. Drop it.
			delete packet;
			delete fht->remove(this);
			pthread_mutex_unlock(&(hmb->mutex));
			return 1;
		}else{
			cal_packet(packet, src);
			delete packet;
			close = TRUE;
			fq->enq(fht->remove(this));
			pthread_mutex_unlock(&(hmb->mutex));
			return 0;
		}
	}

	/* TH_ACK: third handshake */
	if(pkts_src == 1 && src == TRUE){
		if((packet->tcp_flags & TcpHeader::TH_ACK) == TcpHeader::TH_ACK){
			ack2_sec = packet->cap_sec;
			ack2_usec = packet->cap_usec;
			/* round trip time in microsecond */
			rtt = (ack2_sec - syn_sec) * 1000000 + (ack2_usec - syn_usec);

			cal_packet(packet, src);
			delete packet;
			pthread_mutex_unlock(&(hmb->mutex));
			return 0;
		}
	}

	/* TH_FIN:
	* The flow will be closed if the both fins are detected */
	if( (packet->tcp_flags & TcpHeader::TH_FIN) == TcpHeader::TH_FIN){
		if( src == TRUE ){
			close = CLIENT_CLOSE;
		}else{
			close = SERVER_CLOSE;
		}		
		cal_packet(packet, src);
		delete packet;

		if(close == CLIENT_CLOSE  || close == SERVER_CLOSE){		/* && or || */
			/* flow finished and send it to the flow queue */
			close = TRUE;
			fq->enq(fht->remove(this));
		}

		pthread_mutex_unlock(&(hmb->mutex));
		return 0;
	}

	/* other packets, without sequence number checked */
	if(src == TRUE){
		if( pkts_src == 0){
			/* syn */
			syn_sec = packet->cap_sec;
			syn_usec = packet->cap_usec;
			cal_packet(packet, src);
			delete packet;
		}else{
			if(packet->tcp_flags == TcpHeader::TH_SYN){
				/*syn repeatly*/
				reset();		// Reset flow
				syn_sec = packet->cap_sec;
				syn_usec = packet->cap_usec;
				cal_packet(packet, src);
				delete packet;
			}else{
				if(packet->http != 0 ){
					http = TRUE;
					/*
					* only packets with HTTP payload
					* are hooked on the packet chain
					*/
					hook_packet(packet, src);
					cal_packet(packet,src);
				}else{
					cal_packet(packet, src);
					delete packet;
				}
			}
		}
	}else{
		if(packet->http != 0){
			http = TRUE;
			/*
			* only packets with HTTP payload
			* are hooked on the packet chain
			*/
			hook_packet(packet, src);
			cal_packet(packet, src);
		}else{
			cal_packet(packet, src);
			delete packet;
		}
	}

	pthread_mutex_unlock(&(hmb->mutex));
	return 0;
}

/* Extract http_pair_t objects from flow's packet_t chain */
//flow_extract_pairs(f, packet);

// order list source / dest
// add a flag + pointer to point to the last incompleted pair
// order them
// only if response

/* Extract http_pair_t objects from flow's packet_t chain */
int Flow::extract_http(bool closed){
	/* check if the flow is carrying HTTP again */
	if( http == FALSE)
		return 1;
		
	/*
	* Find the actual FIN sequences.
	*/
	Seq	*seq = order->src;
	Seq	*src_fin_seq = NULL;
	Seq	*dst_fin_seq = NULL;
	int found = 0;
	
	while(seq != NULL){
		/* Update flow's first byte time.
		* FBT of flow refers to the payload's FBT.
		*/
		if(seq->pkt != NULL && found == 0 && init == FALSE){
			found = 1;
			fb_sec = seq->cap_sec;
			fb_usec = seq->cap_usec;
			init = TRUE;
		}
		
		/*Search the FIN sequence in sequence queue.*/
		if(seq->th_flags & TcpHeader::TH_FIN == TcpHeader::TH_FIN){
			src_fin_seq = seq;
			break;
		}
		seq = seq->next;
	}
	
	seq = order->dst;
	while(seq != NULL){
		/*Search the FIN sequence in sequence queue.*/
		if(seq->th_flags & TcpHeader::TH_FIN == TcpHeader::TH_FIN){
			dst_fin_seq = seq;
			break;
		}
		seq = seq->next;
	}
	
	/*
	* Set the client and server FIN sequences.
	*/
	Seq	*fin_seq = NULL;	/* The actual FIN sequence. */
	//u_int8_t	fin_dir = 0;	
	/* fin_dir:
	* 0: Not set;
	* 1: src_fin_seq is used;
	* 2: dst_fin_seq is used;
	*/
	
	if (src_fin_seq != NULL && dst_fin_seq == NULL){
		fin_seq = src_fin_seq;
		//fin_dir = 1;
	}else if (src_fin_seq == NULL && dst_fin_seq != NULL){
		fin_seq = dst_fin_seq;
		//fin_dir = 2;
	}else if (src_fin_seq != NULL && dst_fin_seq != NULL){
		fin_seq = src_fin_seq;
		//fin_dir = 1;
		if(src_fin_seq->compare(dst_fin_seq) == 1){
			fin_seq = dst_fin_seq;
			//fin_dir = 2;
		}
	}else{
		fin_seq = NULL;
		//fin_dir = 0;
	}
	
	/* 
	* First step: find requests 
	*/
	Packet *pkt;
	request_t	*req;
	response_t	*rsp;
	int reqn = 0;	// Number of requests.
	int rspn = 0;	// Number of responses.
	
	http_pair_t *new_http = NULL;
	Seq *seq_next = NULL;	/* for temp */
	Seq *first_seq = NULL;
	/* Set seq and seq_next */
	seq = order->src;
	if( seq != NULL){
		seq_next = seq->next;
	}else{
		seq_next = NULL;		/* NULL */
	}
	
	if (fin_seq != NULL && seq != NULL){
		/*A FIN packet exists.*/
		while(seq->compare(fin_seq) < 0){
			pkt = seq->pkt;
			if(pkt != NULL && pkt->http == Packet::HTTP_REQ && !seq->processed){
				//int res = tcp_order_check(order);
				//if(res == 0) printf("Unordered flow \n");
				
				seq->processed = TRUE;

				/* When a new HTTP request is found,
				* create a HTTP pair object, then add the object to
				* flow's HTTP chain.
				*/
				reqn++;
				raw_req++;

				/* new HTTP pair object*/
				new_http = http_new();
				first_seq = seq;
				new_http->req_fb_sec = seq->cap_sec;
				new_http->req_fb_usec = seq->cap_usec;
				new_http->req_lb_sec = seq->cap_sec;
				new_http->req_lb_usec = seq->cap_usec;
					
				/* Add the object to flow's HTTP chain */
				add_http(new_http);
				/* new request object */
				req = http_request_new();
				/* Add the request object to the foregoing HTTP pair object */
				http_add_request(new_http, req);
				/* parse and write values to foregoing request object */
			
	
				/** Added Functionality: assuming there's only
				 *  one request packet, 
				 *  store capture time and sequence + size of  
				 *  packet in http_pair->request->header when 
				 *  header is parsed. 
				**/

				char time[20];
				sprintf(time,"%ld",pkt->cap_usec);
				//http_parse_request(req, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl,time,pkt->tcp_seq+pkt->tcp_dl);
				http_parse_request(req, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl,time,pkt->tcp_seq,pkt->tcp_seq+pkt->tcp_dl);

			}else{
				/* Omit the TCP handshake sequences.*/
				/* or already processed sequence */
				if(new_http == NULL || seq->processed){
					seq = seq->next;
					if(seq != NULL)
						seq_next = seq->next;
					else
						break;
					continue;
				}
			}

			if( new_http != NULL ){
				// Last bit 
				// We check if the flow is closed to detect the last packet
				if( ( seq_next == NULL && closed ) || seq_next == fin_seq || ( seq_next != NULL && seq_next->pkt != NULL ) ||\
				seq_next->compare(fin_seq) >= 0 ){
					if( seq->nxt_seq != 0){
						new_http->req_total_len = seq->nxt_seq - first_seq->seq;
						new_http->req_body_len = 0;
					}
					/*Update flow's last byte time.*/
					if ((seq->cap_sec > lb_sec) || (seq->cap_sec == lb_sec && seq->cap_usec > lb_usec)){
						lb_sec = seq->cap_sec;
						lb_usec = seq->cap_usec;
					}
				}
			}
			
			/* Continue to next sequence.*/
			seq = seq->next;
			if(seq != NULL)
				seq_next = seq->next;
			else
				break;
		}
	}else{
		/* No FIN packet found.*/
		while(seq != NULL){
			pkt = seq->pkt;
			if(pkt != NULL && pkt->http == Packet::HTTP_REQ && !(seq->processed)){

				seq->processed = TRUE;

				/* When a new HTTP request is found,
				* create a HTTP pair object, then add the object to
				* flow's HTTP chain.
				*/
				reqn++;
				raw_req++;

				/* new HTTP pair object*/
				new_http = http_new();
				first_seq = seq;
				new_http->req_fb_sec = seq->cap_sec;
				new_http->req_fb_usec = seq->cap_usec;
				new_http->req_lb_sec = seq->cap_sec;
				new_http->req_lb_usec = seq->cap_usec;
		
				/* Add the object to flow's HTTP chain */
				add_http(new_http);

				/* new request object */
				req = http_request_new();
				/* Add the request object to the foregoing HTTP pair object */
				http_add_request(new_http, req);

				
				/** Added Functionality: assuming there's only
				 *  one request packet, 
				 *  store capture time and sequence + size of  
				 *  packet in http_pair->request->header when 
				 *  header is parsed. 
				**/

				char time[20];
				sprintf(time,"%ld",pkt->cap_usec);
				/* parse and write values to foregoing request object */
				//http_parse_request(req, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl,time,pkt->tcp_seq + pkt->tcp_dl);
				http_parse_request(req, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl,time,pkt->tcp_seq,pkt->tcp_seq+pkt->tcp_dl);
			}else{
				if(new_http == NULL || seq->processed){
					/*Omit the TCP handshake sequences.*/
					seq = seq->next;
					if(seq != NULL)
						seq_next = seq->next;
					else
						break;
					continue;
				}
			}
			if( new_http != NULL ){
				// Last bit
				// We check if the flow is closed to detect the last packet
				if( ( seq_next == NULL && closed ) || ( seq_next != NULL && seq_next->pkt != NULL ) ){
					if( seq->nxt_seq != 0){
						new_http->req_total_len = seq->nxt_seq - first_seq->seq;
						new_http->req_body_len = 0;
					}
					/*Update flow's last byte time.*/
					if ((seq->cap_sec > lb_sec) || (seq->cap_sec == lb_sec && seq->cap_usec > lb_usec)){
						lb_sec = seq->cap_sec;
						lb_usec = seq->cap_usec;
					}
				}
			}
			/*Continue to next sequence.*/
			seq = seq->next;
			if(seq != NULL)
				seq_next = seq->next;
			else
				break;
		}
	}

	/* If no responses found, we treat the flow as invalid and stop parsing */
	if(reqn == 0)
		return 1;
		
	/* Second step: find responses */
	http_pair_t *tmp = http_f;
	http_pair_t *found_http = NULL;
	seq = order->dst;
	if( seq != NULL)
		seq_next = seq->next;
	else
		seq_next = NULL;		/* NULL */
	if(fin_seq != NULL && seq != NULL){
		/*There is FIN packet.*/
		int first_packet = 0;
		while(seq->compare(fin_seq) < 0){
			pkt = seq->pkt;
			if ( pkt != NULL && pkt->http == Packet::HTTP_RSP && !seq->processed){

				seq->processed = TRUE;
				
				/*
				* Similar to the request parsing, a new response is
				* added to the first pair without response
				*/
				rspn++;

				u_int32_t ack = pkt->tcp_ack;
				raw_rsp++;

				/* Try to find the first pair without response */
				while(tmp != NULL){
					/*if(tmp->request_header->nxt_seq > ack) {
						printf("ns: %" PRIu32 " ack: %" PRIu32" \n", tmp->request_header->nxt_seq, ack);
						tmp = NULL;
						break;
					}*/

					if(tmp->response_header == NULL /*&& tmp->request_header->nxt_seq == ack*/) {
						//printf("ns: %" PRIu32 " ack: %" PRIu32" \n", tmp->request_header->nxt_seq, ack);
						break;
					}
					tmp = tmp->next;
				}
				if(tmp == NULL) {
					/* no response empty, then return */
					//d3++;
					return 1;
				}else{
					/*Found!*/
					found_http = tmp;
					first_seq = seq;
					found_http->rsp_fb_sec = seq->cap_sec;
					found_http->rsp_fb_usec = seq->cap_usec;

					rsp = http_response_new();
					http_add_response(found_http, rsp);

					first_packet=1;	
					/** Added functionality: store
					 *  acknowledgement number in 
					 *  response_header information.
					**/
					http_parse_response(rsp, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl, ack);
				}
			}else{
				if(found_http == NULL || seq->processed){
					seq = seq->next;
					if(seq != NULL)
						seq_next = seq->next;
					else
						break;
					continue;
				}
			}

			if ( found_http != NULL ){
				/*first_seq != NULL*/
				
				/** Added functionality: Concatenate capture
				 *  times of all response packets to string
				 *  stored in http_pair, separated by commas.
				**/
				char temp[20];
				sprintf(temp,",%ld",seq->cap_usec);
				strcat(found_http->response_header->time,temp);
				
				if (!first_packet){
					sprintf(temp,",%d",seq->size);
				}
				else{
					sprintf(temp,"%d",seq->size);
				}
				strcat(found_http->response_header->size,temp);

				first_packet=0;
				if( seq_next == NULL || seq_next == fin_seq || seq_next->pkt != NULL ||\
						seq_next->compare(fin_seq) >= 0 ){
					found_http->rsp_lb_sec = seq->cap_sec;
					found_http->rsp_lb_usec = seq->cap_usec;
					
					if(seq->nxt_seq != 0){
						found_http->rsp_total_len = seq->nxt_seq - first_seq->seq;
						found_http->rsp_body_len = found_http->rsp_total_len - found_http->response_header->hdlen;
						if (found_http->rsp_body_len < 0)
						{
							found_http->rsp_body_len = -1;
						}
					}
					/*Update flow's last byte time.*/
					if ((seq->cap_sec > lb_sec) || (seq->cap_sec == lb_sec && seq->cap_usec > lb_usec)){
						lb_sec = seq->cap_sec;
						lb_usec = seq->cap_usec;
					}
				}
			}

			seq = seq->next;
			if(seq != NULL)
				seq_next = seq->next;
			else
				break;
		}
	}else{
		/*There is no FIN packet.*/
		while(seq != NULL){
			int first_packet = 0;
			pkt = seq->pkt;
			if ( pkt != NULL && pkt->http == Packet::HTTP_RSP && !seq->processed){

				seq->processed = TRUE;

				/*
				* Similar to the request parsing, a new response is
				* added to the first pair without response
				*/
				rspn++;

				//u_int32_t ack = pkt->tcp_ack;
				raw_rsp++;
				
				/* Try to find the first pair without response */
				while(tmp != NULL){
					
					if(tmp->response_header == NULL /*&& tmp->request_header->nxt_seq == ack*/) {
						break;
					}
					tmp = tmp->next;
				}
				if(tmp == NULL)
					/* no response empty, then return */
					return 1;
				else{
					/*Found!*/
					first_packet = 1;
					found_http = tmp;
					first_seq = seq;
					found_http->rsp_fb_sec = seq->cap_sec;
					found_http->rsp_fb_usec = seq->cap_usec;
					rsp = http_response_new();
					http_add_response(found_http, rsp);


					/** Added functionality: store
					 *  acknowledgement number in 
					 *  response_header information.
					**/

					http_parse_response(rsp, pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl, pkt->tcp_ack);
				}
			}else{
				if(found_http == NULL || seq->processed){
					seq = seq->next;
					if(seq != NULL)
						seq_next = seq->next;
					else
						break;
					continue;
				}
			}

			if ( found_http != NULL ){
				/*first_seq != null*/                      

				/** Added functionality: Concatenate capture
				 *  times of all response packets to string
				 *  stored in http_pair, separated by commas.
				**/

				char temp[20];
				sprintf(temp,",%ld",seq->cap_usec);
				strcat(found_http->response_header->time,temp);
				if (!first_packet){
					sprintf(temp,",%d",seq->size);
				}else{
					sprintf(temp,"%d",seq->size);
				}
				strcat(found_http->response_header->size,temp);

				first_packet=0;

				if( seq_next == NULL || seq_next->pkt != NULL ){
					found_http->rsp_lb_sec = seq->cap_sec;
					found_http->rsp_lb_usec = seq->cap_usec;
					
					if(seq->nxt_seq != 0){
						found_http->rsp_total_len = seq->nxt_seq - first_seq->seq;	
						found_http->rsp_body_len = found_http->rsp_total_len - found_http->response_header->hdlen;
						if (found_http->rsp_body_len < 0)
						{
							found_http->rsp_body_len = -1;
						}
					}
					
					/*Update flow's last byte time.*/
					if ((seq->cap_sec > lb_sec) || (seq->cap_sec == lb_sec && seq->cap_usec > lb_usec)){
						lb_sec = seq->cap_sec;
						lb_usec = seq->cap_usec;
					}
				}
			}

			seq = seq->next;
			if(seq != NULL)
				seq_next = seq->next;
			else
				break;
		}
	}

	//printf("total rsp: %d \n", rspn);
	return 0;
}