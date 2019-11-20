#include "analysis.hpp"

Analysis::Analysis(int socket, const char* interface, uint32_t interval, uint32_t duration, bool serverMode, bool debug) 
: interface(interface), socket(socket), interval(interval), duration(duration), serverMode(serverMode), int_step(0), hasClientIp(false), hasServerIp(0), debug(debug), status(0) {
    eventManager = new EventManager();
    metricManager = new MetricManager(this);

    /* Initialization of packet and flow data structures */
	rpq = new Queue();
	pq = new Queue();
	fq = new Queue();
	fht = new FlowHashTable(fq);

	eventManager->analysisEnded->add(this);
}

void Analysis::activeMetrics(int activeMetrics) {
	for (int i = metricManager->metrics.size() - 1; i >= 0; i--) {
		bool status = activeMetrics & (1<<i);
		
		if(!status) {
			metricManager->metrics.erase(metricManager->metrics.begin() + i);
		} else {
			// Register metric to its attached events
			metricManager->metrics.at(i)->subscribe(eventManager);
		}	
	}
}

bool Analysis::isServerMode() {
    return serverMode;
}

void Analysis::setClientIp(u_int32_t ip) {
    hasClientIp = true;
    client.ip = ip;				
}

void Analysis::setClientPort(u_int16_t port) {
    hasClientPort = true;
    client.port = port;	
}

void Analysis::setServerIp(u_int32_t ip) {
    hasServerIp = true;
    server.ip = ip;	
}

void Analysis::setServerPort(u_int16_t port) {
    hasServerPort = true;
    server.port = port;	
}

void Analysis::flowHashProcess() {
	Flow* flow, *flow_next = NULL;

	for(int i=0; i<HASH_SIZE; i++){
		pthread_mutex_lock(&(fht->flow_hash_table[i]->mutex));
		flow = fht->flow_hash_table[i]->first;
		
		while(flow != NULL ){
			flow_next = flow->next;
			flow->extract_http(FALSE);
			extractData(flow);
			flow = flow_next;
		}
		
		pthread_mutex_unlock(&(fht->flow_hash_table[i]->mutex));
	}
}

void Analysis::extractData(Flow* flow){	
	if(hasClientIp) {
		if(client.ip != flow->socket.saddr) {
			return;
		}
		
		if(hasClientPort) {
			if(client.port != flow->socket.sport) {
				return;
			}
		}

		if(hasServerIp) {
			if(server.ip != flow->socket.daddr) {
				return;
			}
			
			if(hasServerPort) {
				if(server.port != flow->socket.dport) {
					return;
				}
			}
		}
	}

	if(!flow->processed) {
		flow->processed = TRUE;		
		eventManager->newFlowReceived->notify(flow);		
	}
	
	// Need client/server value
	eventManager->flowUpdate->notify(flow);

	if (flow->pair_f != NULL){        	
		pair_t *pair = flow->pair_f;
		while(pair != NULL) {
			if(pair->request_header != NULL) {
				if(!pair->req_processed) {
					pair->req_processed = TRUE;
					request_t *req = pair->request_header;
					eventManager->requestReceived->notify(pair, flow);
					req_tot++;
				}
				
				if(pair->rsp_processed) {
					pair_t* next = pair->next;

					if(next != NULL) {
						flow->pair_f = next;
						pair_free(pair);
					}
					pair = next;
					continue;
				}

				if(pair->response_header != NULL) {
					pair->rsp_processed = TRUE;
					rsp_tot++;

					eventManager->responseReceived->notify(pair, flow);
				}
			}	   
			pair = pair->next;
		}
	}
}

void Analysis::stop() {
	status = -1;
}

bool Analysis::isStopped() {
	return (status == -1);
}

void Analysis::onTimerExpired() {
}

void Analysis::onIntervalExpired() {
}

void Analysis::onAnalysisEnded() {
	stop();

	if(debug) {
		//printf("pak: %d \n", pak);
		//printf("dpak: %d \n", pak_deq);
		//printf("reqn: %d \n", req_n);
		//printf("rspn: %d \n", rsp_n);
	}
}

void Analysis::onNewFlowReceived(Flow *flow) {}
void Analysis::onFlowUpdate(Flow *flow) {}
void Analysis::onRequestReceived(pair_t *pair, Flow *flow) {}
void Analysis::onResponseReceived(pair_t *pair, Flow *flow) {}