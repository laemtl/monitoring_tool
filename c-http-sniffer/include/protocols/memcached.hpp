#ifndef __MEMCACHED_H__
#define __MEMCACHED_H__

#include <sys/types.h>
#include <time.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <map>
#include "protocol.hpp"

namespace _memcached {

    class MemCached : public _protocol::Protocol {
        public:
            MemCached(Analysis* analysis);
            
            bool isHeaderPacket(const char *ptr, const int datalen);
            char* isRequest(const char *p, const int datalen);	    /* If the packet carries HTTP request data */
            char* isResponse(const char *p, const int datalen);	    /* If the packet carries HTTP response data */

            int extractPair(Flow* flow, bool closed);
            _protocol::Request* getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);
            _protocol::Response* getResponse(const char *data, const char *dataend, long ack);
    };

    class Request : public _protocol::Request, public RequestMethod  {
        public:
            u_int32_t   seq;
            u_int32_t   nxt_seq;
            std::vector<const char*> keys;
            char*       time;
            u_int32_t expiration;
            u_int8_t  flags; 

            Request();
            Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);   /* Parse the packet and store in a Request object */
            ~Request();
            int parseMethod(const char *data, int linelen);
    };

    /*
    * Cache response header
    */
    class Response : public _protocol::Response, public ResponseStatus {
        public:
            long acknowledgement;
            std::vector<const char*> keys;
            u_int8_t  flags;
            u_int32_t bodylen;

            Response();
            Response(const char *data, const char *dataend, long ack);	/* Parse the packet and store in a Response object */
            ~Response();
            int parseStatus(const char *line, int len);
            bool hasErrorStatus();
    };
}

#include "packet.h"
#include "util.h"
#include "responseStatus.hpp"
#include "requestMethod.hpp"

using namespace _memcached;

#endif
