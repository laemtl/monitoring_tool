#include "flow.hpp"

//extern int raw_req;
//extern int raw_rsp;

/* Create a new flow record and initiate its members */
Flow::Flow(Queue* fq, FlowHashTable* fht) 
: fht(fht), fq(fq), pkt_src_n(0), pkt_dst_n(0), pkts_src(0), init(false), pair_f(NULL), lb_sec(0), lb_usec(0), processed(false) {
    order = order_new();
}

/* Reset a Flowobject to reuse */
void Flow::reset(){
    packet_t *cp;

    while(pkt_dst_f != NULL){
        cp = pkt_dst_f;
        pkt_dst_f = pkt_dst_f->next;
        packet_free(cp);
    }

    while(pkt_src_f != NULL){
        cp = pkt_src_f;
        pkt_src_f = pkt_src_f->next;
        packet_free(cp);
    }

    if(order != NULL){
        order_free(order);
        order = order_new();
    }
    /*
    * Note: maintain the socket.
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
    close = 0;
}

/* Serve for flow_add_packet(). Add packet to the flow packet chain. */
int Flow::hook_packet(packet_t *packet, bool src){
    /* set some pointers */
    packet_t **pkt_f = NULL, **pkt_e = NULL;
    u_int32_t *pkt_num = NULL;

    if(src == true){
        pkt_f = &(pkt_src_f);
        pkt_e = &(pkt_src_e);
        pkt_num = &(pkt_src_n);
    } else {
        pkt_f = &(pkt_dst_f);
        pkt_e = &(pkt_dst_e);
        pkt_num = &(pkt_dst_n);
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
Manage the pair processing
*/
int Flow::cal_packet(packet_t *packet, bool src){
    u_int32_t *pkt_num = NULL, *byt_num = NULL;
    struct timeval tv;
    struct timezone tz;
    seq_t *seq = NULL;

    if(src == true){
        pkt_num = &(pkts_src);
        byt_num = &(payload_src);
    }else{
        pkt_num = &(pkts_dst);
        byt_num = &(payload_dst);
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
    seq = seq_pkt(packet);
    if ( packet->type == 0 ){
        /*
        * The packet without payload will NOT be stored.
        */
        seq->pkt = NULL;
    }

    tcp_order(order, seq, src);
    //flow_extract_pairs(f, seq);

    return 0;
}

/* Compare seq_t objects if equel */
int Flow::compare_sequence_time(seq_t *seq1, seq_t *seq2){
    u_int32_t	sec1 = seq1->cap_sec;
    u_int32_t	usec1 = seq1->cap_usec;
    u_int32_t	sec2 = seq2->cap_sec;
    u_int32_t	usec2 = seq2->cap_usec;
    int ret = 0;

    if(sec1 > sec2 || (sec1 == sec2 && usec1 > usec2)){
        ret = 1;
    }else if (sec1 < sec2 || (sec1 == sec2 && usec1 < usec2)){
        ret = -1;
    }else{
        ret = 0;
    }

    return ret;
}

/* Free a Flowobject */
Flow::~Flow(){
    packet_t *cp;
    Pair	*h;
    while(pkt_dst_f != NULL){
        cp = pkt_dst_f;
        pkt_dst_f = pkt_dst_f->next;
        packet_free(cp);
    }

    while(pkt_src_f != NULL){
        cp = pkt_src_f;
        pkt_src_f = pkt_src_f->next;
        packet_free(cp);
    }

    if(order != NULL){
        order_free(order);
    }

    while(pair_f != NULL){
        h = pair_f;
        pair_f = pair_f->next;
        delete h;
    }
}

/* Compare two flow_s objects */
int Flow::flow_socket_cmp(flow_s *fs){
    return memcmp(&socket, fs, sizeof(flow_s));
}

/* Add a Pair object to flow's Pair chain */
int Flow::add_pair(Pair *h){
    if(pair_f == NULL){
        pair_f = h;
    }else{
        pair_e->next = h;
    }
    pair_e = h;
    pair_e->next = NULL;
    return 0;
}

/* Add a packet_t object to flow's packet_t chain */
int Flow::add_packet(packet_t *packet, register bool src){
    pthread_mutex_lock(&(hmb->mutex));

    /* TH_RST:
    * If the flow is reset by sender or receiver*/
    if((packet->tcp_flags & TH_RST) == TH_RST){
        if( pkts_src < 4){
            // Flow with uncomplete information. Drop it.
            packet_free(packet);
            delete fht->remove(this);
            pthread_mutex_unlock(&(hmb->mutex));
            return 1;
        }else{
            packet->type = 0;
            cal_packet(packet, src);
            packet_free(packet);
            close = true;
            fq->enq(fht->remove(this));
            pthread_mutex_unlock(&(hmb->mutex));
            return 0;
        }
    }

    /* TH_ACK: third handshake */
    if(pkts_src == 1 && src == true){
        if((packet->tcp_flags & TH_ACK) == TH_ACK){
            ack2_sec = packet->cap_sec;
            ack2_usec = packet->cap_usec;
            /* round trip time in microsecond */
            rtt = (ack2_sec - syn_sec) * 1000000 + (ack2_usec - syn_usec);
            
            packet->type = 0;
            cal_packet(packet, src);
            packet_free(packet);
            
            pthread_mutex_unlock(&(hmb->mutex));
            return 0;
        }
    }

    /* TH_FIN:
    * The flow will be closed if the both fins are detected */
    if( (packet->tcp_flags & TH_FIN) == TH_FIN){
        if( src == true ){
            close = CLIENT_CLOSE;
        }else{
            close = SERVER_CLOSE;
        }	

        packet->type = 0;
        cal_packet(packet, src);
        packet_free(packet);

        if(close == CLIENT_CLOSE  || close == SERVER_CLOSE){		/* && or || */
            /* flow finished and send it to the flow queue */
            close = true;
            fq->enq(fht->remove(this));
        }

        pthread_mutex_unlock(&(hmb->mutex));
        return 0;
    }

    /* other packets, without sequence number checked */
    if(src == true){
        //if(pkts_src == 0){
        if(packet->tcp_flags == TH_SYN && pkts_src == 0){
            /* syn */
            syn_sec = packet->cap_sec;
            syn_usec = packet->cap_usec;

            packet->type = 0;
            cal_packet(packet, src);
            packet_free(packet);
        } else {
            if(packet->tcp_flags == TH_SYN){
                /*syn repeatly*/
                reset();		// Reset flow
                syn_sec = packet->cap_sec;
                syn_usec = packet->cap_usec;

                packet->type = 0;
                cal_packet(packet, src);
                packet_free(packet);
            } else {
                if(packet->type != 0 ){
                    /*
                    * only packets with HTTP payload
                    * are hooked on the packet chain
                    */
                    hook_packet(packet, src);
                    cal_packet(packet,src);
                }else{
                    packet->type = 0;
                    cal_packet(packet, src);
                    packet_free(packet);
                }
            }
        }
    }else{
        if(packet->type != 0){
            /*
            * only packets with payload
            * are hooked on the packet chain
            */
            hook_packet(packet, src);
            cal_packet(packet, src);
        }else{
            packet->type = 0;
            cal_packet(packet, src);
            packet_free(packet);
        }
    }

    pthread_mutex_unlock(&(hmb->mutex));
    return 0;
}