#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <sys/time.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <vector>

class Analysis;
class FlowQueue;
class FlowHashTable;
class EventManager;
class PacketQueue;
class Queue;
class Flow;
class Metric;

namespace _protocol {

    class Request {
        public:
            /** Added functionality: Request header stores
             *  capture time as a string and sequence number
             *  and tcp length.
            **/
            char* 		time;
            int			hdlen;	// Header length
            bool        processed;
            u_int32_t   seq;
            u_int32_t   nxt_seq;
            
            Request();
            Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);
            //~Request();
    };

    class Response {
        public:
            /** Added functionality: Response header stores
             *  capture time of all response packets, as well
             *  as acknowledgement number.
             *  Capture times separated by commas. 
             *  ((WARNING))
             *  If too many response packets, time string may overflow.
            **/
            char			time[256] = {0};
            char			size[256] = {0};
            int				hdlen;	// Header length
            bool            processed;
         
            Response();
            Response(const char *data, const char *dataend, long ack);
            //~Response();
    };

    class Pair {
        public:
            Request	 *request_header;
            Response *response_header;
            time_t	req_fb_sec;
            time_t	req_fb_usec;
            time_t	req_lb_sec;
            time_t	req_lb_usec;
            time_t	rsp_fb_sec;
            time_t	rsp_fb_usec;
            time_t	rsp_lb_sec;
            time_t	rsp_lb_usec;
            u_int32_t	req_total_len;
            u_int32_t	rsp_total_len;
            u_int32_t	req_body_len;
            u_int32_t	rsp_body_len;
            Pair	*next;

            Pair();
            int addRequest(Request *req);	/* Add a Request object to Pair request chain */
            int addResponse(Response *rsp);	/* Add a Response object to Pair response chain */
    };

    class Protocol {
        public:
            char* name;
            uint32_t id;
            EventManager* eventManager;
            std::vector<Metric*> metrics;
            Analysis* analysis;
            int req_tot;
            int rsp_tot;
            int flow_tot;
        
            // flow queue
            Queue* fq;

            // packet queue
            Queue* pq;

            // flow hash table
            FlowHashTable* fht;

            bool hasClientIp;
            u_int32_t	clientIp;
            std::vector<u_int16_t>	clientPorts;

            bool hasServerIp;
            u_int32_t	serverIp;
            std::vector<u_int16_t> serverPorts;

            Protocol(Analysis* analysis, char* protocolName, uint32_t protocolId);
            bool isHeaderPacket(const char *ptr, const int datalen);
            virtual char* isRequest(const char *p, const int datalen) = 0;
            virtual char* isResponse(const char *p, const int datalen) = 0;
            virtual Request* getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) = 0;
            virtual Response* getResponse(const char *data, const char *dataend, long ack) = 0;     
            bool isPacketOf(u_int16_t sport, u_int16_t dport);
            void flowHashProcess();
            int extractPair(Flow* flow, bool closed);
            void extractData(Flow* flow);
            void activeMetrics(int activeMetrics);
            static char* header_param(const char *header, int hlen, const char *param);
            static char* find_line_end(const char *data, const char *dataend, const char **eol);
            static int get_token_len(const char *linep, const char *lineend, const char **next_token);
            static char* find_header_end(const char *data, const char *dataend, int *line_cnt);
            void onAnalysisEnded();
            void setClientIp(u_int32_t ip);
            void addClientPort(u_int16_t port);
            void setServerIp(u_int32_t ip);
            void addServerPort(u_int16_t port);
        
    };
}

#include "metric.hpp"
#include "eventManager.hpp"
#include "analysis.hpp"
#include "packetQueue.hpp"
#include "flow.hpp"
#include "flowHashTable.hpp"
#include "flowQueue.hpp"
#include "queue.hpp"

#include "rst.hpp"
#include "reqRate.hpp"
#include "errRate.hpp"
#include "tp.hpp"
#include "tpRev.hpp"
#include "connRate.hpp"
#include "client.hpp"
#include "reqPath.hpp"
#include "reqMethod.hpp"
#include "reqType.hpp"
#include "rspStatus.hpp"

using namespace _protocol;

#endif