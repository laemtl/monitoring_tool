#include "protocol.hpp"

int	Pair::addRequest(Request *req) {
    if(request_header == NULL){
		request_header = req;
		return 0;
	} else {
		return 1;
	}
}

Pair::Pair() 
: request_header(NULL), response_header(NULL), req_fb_sec(0), req_fb_usec(0), req_lb_sec(0), 
req_lb_usec(0), rsp_fb_sec(0), rsp_fb_usec(0), rsp_lb_sec(0), rsp_lb_usec(0), req_total_len(0), 
rsp_total_len(0), req_body_len(0), rsp_body_len(0), next(NULL) {
}

_protocol::Request::Request() : time(NULL), hdlen(0), processed(false) {
}

// override
_protocol::Request::Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) : Request() {
}

_protocol::Response::Response() : hdlen(0), processed(false) {
}
           
_protocol::Response::Response(const char *data, const char *dataend, long ack) : Response() {           
}

int Pair::addResponse(Response *rsp) {
	if(response_header == NULL){
		response_header = rsp;
		return 0;
	} else {
		return 1;
	}
}

Protocol::Protocol(Analysis* analysis, char* protocolName, uint32_t protocolId) : analysis(analysis), hasClientIp(false), hasServerIp(false), name(protocolName), id(protocolId) {
    eventManager = new EventManager();
    
    /* Initialization of packet and flow data structures */
    pq = new Queue();
    fq = new Queue();
    fht = new FlowHashTable(fq);
}
        
void Protocol::flowHashProcess() {
    Flow* flow, *flow_next = NULL;

    for(int i=0; i<HASH_SIZE; i++){
        pthread_mutex_lock(&(fht->flow_hash_table[i]->mutex));
        flow = fht->flow_hash_table[i]->first;
        
        while(flow != NULL ){
            flow_next = flow->next;
            extractPair(flow, false);
            extractData(flow);
            flow = flow_next;
        }
        
        pthread_mutex_unlock(&(fht->flow_hash_table[i]->mutex));
    }
}

void Protocol::extractData(Flow* flow){	
    if(hasClientIp && clientIp != flow->socket.saddr) {
        return;
    }
    
    if (clientPorts.size() && !std::count(clientPorts.begin(), clientPorts.end(), flow->socket.sport)) {
        return;
    }
    
    if(hasServerIp && serverIp != flow->socket.daddr) {
        return;
    }
    
    if (serverPorts.size() && !std::count(serverPorts.begin(), serverPorts.end(), flow->socket.dport) && !std::count(serverPorts.begin(), serverPorts.end(), flow->socket.sport)) {
        return;
    }

    if(!flow->processed) {
        flow->processed = true;		
        eventManager->newFlowReceived->notify(flow);		
    }
    
    // Need client/server value
    eventManager->flowUpdate->notify(flow);

    if (flow->pair_f != NULL){
        Pair *pair = flow->pair_f;
        while(pair != NULL) {
            if(pair->request_header != NULL) {
                if(!pair->request_header->processed) {
                    pair->request_header->processed = true;
                    Request *req = pair->request_header;
                    eventManager->requestReceived->notify(pair, flow);
                    req_tot++;
                }
            
                if(pair->response_header != NULL) {
                    if(pair->response_header->processed) {
                        Pair* next = pair->next;

                        if(next != NULL) {
                            flow->pair_f = next;
                            delete pair;
                        }
                        pair = next;
                        continue;
                    }

                    pair->response_header->processed = true;
                    rsp_tot++;

                    eventManager->responseReceived->notify(pair, flow);
                }
            }	   
            pair = pair->next;
        }
    }
}

void Protocol::activeMetrics(int activeMetrics) {
    if(metrics.empty()) return;

    for (int i = metrics.size() - 1; i >= 0; i--) {
        bool status = activeMetrics & (1<<i);
        
        if(!status) {
            metrics.erase(metrics.begin() + i);
        } else {
            // Register metric to its attached events
            metrics.at(i)->subscribe(eventManager);
        }	
    }
}

/*
 * From xplico.
 * Parse header parameter from HTTP header fields.
 * Return the pointer to the parameter value if found;
 * else return NULL.
 */
char* Protocol::header_param(const char *header, int hlen, const char *param) {
    const char *line, *eol, *lineend, *hend, *c;
    char *ret;
    int len, host_len, param_len;

    hend = header + hlen - 1;
    line = header;
    len = hlen;
    ret = NULL;
    lineend = NULL;
    param_len = strlen(param);
    while (lineend < hend) {
        lineend = find_line_end(line, line+len-1, &eol);
        if (lineend != hend && (*eol == '\r' || *eol == '\n')) {
            if (strncasecmp(line, param, param_len) == 0) {
                c = line + param_len;
                while (*c == ' ' && c < lineend)
                    c++;
                /*
                 * Move the EOL pointer to the last none-LFCR character.
                 */
                while ( (*eol == '\r' || *eol == '\n') && eol > c)
                	eol--;
                host_len = eol - c + 1;
                ret = MALLOC(char, host_len + 1);
                memset(ret, '\0', host_len + 1);
                memcpy(ret, c, host_len);
                break;
            }
        }
        line = lineend + 1;
        len = hend - lineend;
    }
    return ret;
}

/*
 * From xplico.
 * Given a pointer into a data buffer and the length of buffer,
 * find the header end.
 * Return a pointer to the end character of header
 */
char* Protocol::find_header_end(const char *data, const char *dataend, int *line_cnt) {
    const char *lf, *nxtlf, *end;

    end = NULL;
    lf = (const char*) memchr(data, '\n', (dataend - data + 1));
    if (lf == NULL)
        return NULL;
    (*line_cnt)++;
    lf++; /* next charater */
    nxtlf = (const char*) memchr(lf, '\n', (dataend - lf + 1));
    while (nxtlf != NULL) {
        if (nxtlf-lf < 2) {
            end = nxtlf;
            break;
        }
        (*line_cnt)++;
        nxtlf++;
        lf = nxtlf;
        nxtlf = (const char*) memchr(nxtlf, '\n', dataend - nxtlf + 1);
    }
    return (char *)end;
}

/*
 * From xplico.
 * Get the length of the next token in a line, and the beginning of the
 * next token after that (if any).
 * Return 0 if there is no next token.
 */
int Protocol::get_token_len(const char *linep, const char *lineend, const char **next_token) {
    const char *tokenp;
    int token_len;

    tokenp = linep;

    /*
     * Search for a blank, a CR or an LF, or the end of the buffer.
     */
    while (linep < lineend && *linep != ' ' && *linep != '\r' && *linep != '\n')
        linep++;
    token_len = linep - tokenp;

    /*
     * Skip trailing blanks.
     */
    while (linep < lineend && *linep == ' ')
        linep++;

    *next_token = linep;

    return token_len;
}

/*
 * From xplico.
 * Given a pointer into a data buffer, and to the end of the buffer,
 * find the end of the line at that position in the data
 * buffer.
 * Return a pointer to the EOL character(s) in "*eol", which is the first of
 * EOL character(s).
 */
char* Protocol::find_line_end(const char *data, const char *dataend, const char **eol) {
	const char *lineend;

	lineend = (const char*)memchr(data, '\n', dataend - data + 1);

	if (lineend == NULL) {
		/*
		 * No LF - line is probably continued in next TCP segment.
		 */
		lineend = dataend;
		*eol = dataend;
	} else {
		/*
		 * Is the LF at the beginning of the line?
		 */
		if (lineend > data) {
			/*
			 * No - is it preceded by a carriage return?
			 * (Perhaps it's supposed to be, but that's not guaranteed....)
			 */
			if (*(lineend - 1) == '\r') {
				/*
				 * Yes.  The EOL starts with the CR.
				 */
				*eol = lineend - 1;

			} else {
				/*
				 * No.  The EOL starts with the LF.
				 */
				*eol = lineend;

				/*
				 * I seem to remember that we once saw lines ending with LF-CR
				 * in an HTTP request or response, so check if it's *followed*
				 * by a carriage return.
				 */
				if (lineend < (dataend - 1) && *(lineend + 1) == '\r') {
					/*
					 * It's <non-LF><LF><CR>; say it ends with the CR.
					 */
					lineend++;
				}
			}
		} else {

			/*
			 * Yes - the EOL starts with the LF.
			 */
			*eol = lineend;
		}
	}
	return (char*)lineend;
}

void Protocol::onAnalysisEnded() {
	fht->clear();
	fht->reset();
}


/* Extract Pair objects from flow's packet_t chain */
//flow_extract_pairs(f, packet);

// order list source / dest
// add a flag + pointer to point to the last incompleted pair
// order them
// only if response

/* Extract Pair objects from flow's packet_t chain */
int Protocol::extractPair(Flow* flow, bool closed){    
    /*
    * Find the actual FIN sequences.
    */
    seq_t	*seq = flow->order->src;
    seq_t	*src_fin_seq = NULL;
    seq_t	*dst_fin_seq = NULL;
    int found = 0;
    
    while(seq != NULL){
        /* Update flow's first byte time.
        * FBT of flow refers to the payload's FBT.
        */
        if(seq->pkt != NULL && found == 0 && flow->init == false){
            found = 1;
            flow->fb_sec = seq->cap_sec;
            flow->fb_usec = seq->cap_usec;
            flow->init = true;
        }
        
        /*Search the FIN sequence in sequence queue.*/
        if(seq->th_flags & TH_FIN == TH_FIN){
            src_fin_seq = seq;
            break;
        }
        seq = seq->next;
    }
    
    seq = flow->order->dst;
    while(seq != NULL){
        /*Search the FIN sequence in sequence queue.*/
        if(seq->th_flags & TH_FIN == TH_FIN){
            dst_fin_seq = seq;
            break;
        }
        seq = seq->next;
    }
    
    /*
    * Set the client and server FIN sequences.
    */
    seq_t	*fin_seq = NULL;	/* The actual FIN sequence. */
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
        if(flow->compare_sequence_time(src_fin_seq, dst_fin_seq) == 1){
            fin_seq = dst_fin_seq;
            //fin_dir = 2;
        }
    } else {
        fin_seq = NULL;
        //fin_dir = 0;
    }
    
    /* 
    * First step: find requests 
    */
    packet_t *pkt;
    Request	*req;
    Response	*rsp;
    int reqn = 0;	// Number of requests.
    int rspn = 0;	// Number of responses.
    
    Pair *new_pair = NULL;
    seq_t *seq_next = NULL;	/* for temp */
    seq_t *first_seq = NULL;
    /* Set seq and seq_next */
    seq = flow->order->src;
    if( seq != NULL){
        seq_next = seq->next;
    } else {
        seq_next = NULL;		/* NULL */
    }
    
    if (fin_seq != NULL && seq != NULL){
        /*A FIN packet exists.*/
        while(flow->compare_sequence_time(seq, fin_seq) < 0){
            pkt = seq->pkt;
            if(pkt != NULL && pkt->type == REQ && !seq->processed){
                seq->processed = true;

                /* When a new HTTP request is found,
                * create a HTTP pair object, then add the object to
                * flow's HTTP chain.
                */
                reqn++;
                //raw_req++;

                /* new HTTP pair object*/
                new_pair = new Pair();
                first_seq = seq;
                new_pair->req_fb_sec = seq->cap_sec;
                new_pair->req_fb_usec = seq->cap_usec;
                new_pair->req_lb_sec = seq->cap_sec;
                new_pair->req_lb_usec = seq->cap_usec;
                    
                /* Add the object to flow's HTTP chain */
                flow->add_pair(new_pair);
                /* new request object */
                
                /** Added Functionality: assuming there's only
                 *  one request packet, 
                 *  store capture time and sequence + size of  
                 *  packet in pair->request->header when 
                 *  header is parsed. 
                **/
                char time[20];
                sprintf(time, "%ld", pkt->cap_usec);

                req = getRequest(pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl, time, pkt->tcp_seq, pkt->tcp_seq+pkt->tcp_dl);
                /* Add the request object to the foregoing HTTP pair object */
                new_pair->addRequest(req);                
            } else {
                /* Omit the TCP handshake sequences.*/
                /* or already processed sequence */
                if(new_pair == NULL || seq->processed){
                    seq = seq->next;
                    if(seq != NULL)
                        seq_next = seq->next;
                    else
                        break;
                    continue;
                }
            }

            if( new_pair != NULL ){
                // Last bit 
                // We check if the flow is closed to detect the last packet
                if( ( seq_next == NULL && closed ) || seq_next == fin_seq || ( seq_next != NULL && seq_next->pkt != NULL ) ||\
                flow->compare_sequence_time(seq_next, fin_seq) >= 0 ){
                    if( seq->nxt_seq != 0){
                        new_pair->req_total_len = seq->nxt_seq - first_seq->seq;
                        new_pair->req_body_len = 0;
                    }
                    /*Update flow's last byte time.*/
                    if ((seq->cap_sec > flow->lb_sec) || (seq->cap_sec == flow->lb_sec && seq->cap_usec > flow->lb_usec)){
                        flow->lb_sec = seq->cap_sec;
                        flow->lb_usec = seq->cap_usec;
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
    } else {
        /* No FIN packet found.*/
        while(seq != NULL){
            pkt = seq->pkt;

            if(pkt != NULL && pkt->type == REQ && !seq->processed){
                seq->processed = true;
                                
                /* When a new HTTP request is found,
                * create a HTTP pair object, then add the object to
                * flow's HTTP chain.
                */
                reqn++;
                //raw_req++;

                /* new HTTP pair object*/
                new_pair = new Pair();
                first_seq = seq;
                new_pair->req_fb_sec = seq->cap_sec;
                new_pair->req_fb_usec = seq->cap_usec;
                new_pair->req_lb_sec = seq->cap_sec;
                new_pair->req_lb_usec = seq->cap_usec;
        
                /* Add the object to flow's HTTP chain */
                flow->add_pair(new_pair);
                /* new request object */

                /** Added Functionality: assuming there's only
                 *  one request packet, 
                 *  store capture time and sequence + size of  
                 *  packet in pair->request->header when 
                 *  header is parsed. 
                **/
                char time[20];
                sprintf(time, "%ld",pkt->cap_usec);

                req = getRequest(pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl,time,pkt->tcp_seq,pkt->tcp_seq+pkt->tcp_dl);
                /* Add the request object to the foregoing HTTP pair object */
                new_pair->addRequest(req);
            } else {
                /* Omit the TCP handshake sequences.*/
                /* or already processed sequence */
                if(new_pair == NULL || seq->processed){
                    seq = seq->next;
                    if(seq != NULL)
                        seq_next = seq->next;
                    else
                        break;
                    continue;
                }
            }
            if( new_pair != NULL ){
                // Last bit
                // We check if the flow is closed to detect the last packet
                if( ( seq_next == NULL && closed ) || ( seq_next != NULL && seq_next->pkt != NULL ) ){
                    if( seq->nxt_seq != 0){
                        new_pair->req_total_len = seq->nxt_seq - first_seq->seq;
                        new_pair->req_body_len = 0;
                    }
                    /*Update flow's last byte time.*/
                    if ((seq->cap_sec > flow->lb_sec) || (seq->cap_sec == flow->lb_sec && seq->cap_usec > flow->lb_usec)){
                        flow->lb_sec = seq->cap_sec;
                        flow->lb_usec = seq->cap_usec;
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

    /* If no request found, we treat the flow as invalid and stop parsing */
    if(reqn == 0)
        return 1;
        
    /* Second step: find responses */
    Pair *tmp = flow->pair_f;
    Pair *found_pair = NULL;
    seq = flow->order->dst;
    if( seq != NULL)
        seq_next = seq->next;
    else
        seq_next = NULL;		/* NULL */
    
    if(fin_seq != NULL && seq != NULL){
        /*There is FIN packet.*/
        int first_packet = 0;
        while(flow->compare_sequence_time(seq, fin_seq) < 0){
            pkt = seq->pkt;
            if ( pkt != NULL && pkt->type == RSP && !seq->processed){
                seq->processed = true;
                
                /*
                * Similar to the request parsing, a new response is
                * added to the first pair without response
                */
                rspn++;

                u_int32_t ack = pkt->tcp_ack;
                bool found = false;
                //raw_rsp++;

                /* Try to find the first pair without response */
                while(tmp != NULL && tmp->request_header->nxt_seq <= ack){
                    if(tmp->response_header == NULL && tmp->request_header->nxt_seq == ack) {
                        found = true;
                        break;
                    }
                    tmp = tmp->next;
                }
                if(tmp == NULL) {
                    /* no more empty response, return */
                    return 1;
                } else if(found) {
                    /*Found!*/
                    found_pair = tmp;
                    first_seq = seq;
                    found_pair->rsp_fb_sec = seq->cap_sec;
                    found_pair->rsp_fb_usec = seq->cap_usec;

                    /** Added functionality: store
                     *  acknowledgement number in 
                     *  response_header information.
                    **/
                    rsp = getResponse(pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl, ack);
                    found_pair->addResponse(rsp);
                    first_packet = 1;	
                }
            } else {
                if(found_pair == NULL || seq->processed){
                    seq = seq->next;
                    if(seq != NULL)
                        seq_next = seq->next;
                    else
                        break;
                    continue;
                }
            }

            if ( found_pair != NULL ){
                /*first_seq != NULL*/
                
                /** Added functionality: Concatenate capture
                 *  times of all response packets to string
                 *  stored in pair, separated by commas.
                **/
                char temp[20];
                sprintf(temp,",%ld",seq->cap_usec);
                strcat(found_pair->response_header->time, temp);
                
                if (!first_packet){
                    sprintf(temp,",%d",seq->size);
                }
                else{
                    sprintf(temp,"%d",seq->size);
                }
                strcat(found_pair->response_header->size, temp);

                first_packet=0;
                if( seq_next == NULL || seq_next == fin_seq || seq_next->pkt != NULL ||\
                        flow->compare_sequence_time(seq_next, fin_seq) >= 0 ){
                    found_pair->rsp_lb_sec = seq->cap_sec;
                    found_pair->rsp_lb_usec = seq->cap_usec;
                    
                    if(seq->nxt_seq != 0){
                        found_pair->rsp_total_len = seq->nxt_seq - first_seq->seq;
                        found_pair->rsp_body_len = found_pair->rsp_total_len - found_pair->response_header->hdlen;
                        if (found_pair->rsp_body_len < 0)
                        {
                            found_pair->rsp_body_len = -1;
                        }
                    }
                    /*Update flow's last byte time.*/
                    if ((seq->cap_sec > flow->lb_sec) || (seq->cap_sec == flow->lb_sec && seq->cap_usec > flow->lb_usec)){
                        flow->lb_sec = seq->cap_sec;
                        flow->lb_usec = seq->cap_usec;
                    }
                }
            }

            seq = seq->next;
            if(seq != NULL)
                seq_next = seq->next;
            else
                break;
        }
    } else {
        /*There is no FIN packet.*/
        int first_packet = 0;
        while(seq != NULL){
            pkt = seq->pkt;
            if ( pkt != NULL && pkt->type == RSP && !seq->processed){

                seq->processed = true;

                /*
                * Similar to the request parsing, a new response is
                * added to the first pair without response
                */
                rspn++;

                u_int32_t ack = pkt->tcp_ack;
                bool found = false;
                //raw_rsp++;
                
                /* Try to find the first pair without response */
                while(tmp != NULL && tmp->request_header->nxt_seq <= ack){
                    if(tmp->response_header == NULL && tmp->request_header->nxt_seq == ack) {
                        found = true;
                        break;
                    }
                    tmp = tmp->next;
                }
                if(tmp == NULL) {
                    /* no response empty, then return */
                    return 1;
                } else if(found) {
                    /*Found!*/
                    first_packet = 1;
                    found_pair = tmp;
                    first_seq = seq;

                    found_pair->rsp_fb_sec = seq->cap_sec;
                    found_pair->rsp_fb_usec = seq->cap_usec;

                    /** Added functionality: store
                     *  acknowledgement number in 
                     *  response_header information.
                    **/
                    rsp = getResponse(pkt->tcp_odata, pkt->tcp_odata + pkt->tcp_dl, pkt->tcp_ack);
                    found_pair->addResponse(rsp);
                }
            } else {
                if(found_pair == NULL || seq->processed){
                    seq = seq->next;
                    if(seq != NULL)
                        seq_next = seq->next;
                    else
                        break;
                    continue;
                }
            }

            if ( found_pair != NULL ){
                /*first_seq != null*/                      

                /** Added functionality: Concatenate capture
                 *  times of all response packets to string
                 *  stored in pair, separated by commas.
                **/

                char temp[20];
                sprintf(temp,",%ld",seq->cap_usec);
                strcat(found_pair->response_header->time, temp);
                if (!first_packet){
                    sprintf(temp,",%d",seq->size);
                } else {
                    sprintf(temp,"%d",seq->size);
                }
                strcat(found_pair->response_header->size, temp);

                first_packet=0;

                if( seq_next == NULL || seq_next->pkt != NULL ){
                    found_pair->rsp_lb_sec = seq->cap_sec;
                    found_pair->rsp_lb_usec = seq->cap_usec;
                    
                    if(seq->nxt_seq != 0){
                        found_pair->rsp_total_len = seq->nxt_seq - first_seq->seq;	
                        found_pair->rsp_body_len = found_pair->rsp_total_len - found_pair->response_header->hdlen;
                        if (found_pair->rsp_body_len < 0)
                        {
                            found_pair->rsp_body_len = -1;
                        }
                    }
                    
                    /*Update flow's last byte time.*/
                    if ((seq->cap_sec > flow->lb_sec) || (seq->cap_sec == flow->lb_sec && seq->cap_usec > flow->lb_usec)){
                        flow->lb_sec = seq->cap_sec;
                        flow->lb_usec = seq->cap_usec;
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

    return 0;
}

void Protocol::setClientIp(u_int32_t ip) {
    hasClientIp = true;
    clientIp = ip;				
}

void Protocol::addClientPort(u_int16_t port) {
    clientPorts.push_back(port);	
}

void Protocol::setServerIp(u_int32_t ip) {
    hasServerIp = true;
    serverIp = ip;	
}

void Protocol::addServerPort(u_int16_t port) {
    serverPorts.push_back(port);	
}

bool Protocol::isPacketOf(u_int16_t sport, u_int16_t dport) {
    if(find(serverPorts.begin(), serverPorts.end(), sport) != serverPorts.end()
    || find(serverPorts.begin(), serverPorts.end(), dport) != serverPorts.end()) {
		return true;
	}
    return false;
}