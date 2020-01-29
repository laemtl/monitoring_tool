#ifndef __MEMCACHED_H__
#define __MEMCACHED_H__

#include <sys/types.h>
#include <time.h>
#include <vector>
#include "packet.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <map>
#include "protocol.hpp"

namespace _memcached {
    enum Status {
        VALUE,
        OK,
        END,
        STORED,
        NOT_STORED,
        EXISTS,
        NOT_FOUND,
        ERROR,
        CLIENT_ERROR,
        SERVER_ERROR,
        DELETED,
        TOUCHED,
        BUSY,
        BADCLASS,
        NOTFULL,
        UNSAFE,
        SAME,
        STAT,
        VERSION,
        CACHE_ST_NONE
    };

    static std::map<const char*, Status> statusCode;
    
    enum Method {
        get,
        set,
        add,
        replace,
        append,
        prepend,
        deletem,
        flush_all,
        quit,
        incr,
        decr,
        touch,
        gat,
        gats,
        slabs,
        lru,
        watch,
        stats,
        version,
        misbehave,
        CACHE_MT_NONE
    };

    Status parseStatus(const char *line, int len);
    Method parseMethod(const char *data, int linelen);  

    class MemCached : public _protocol::Protocol {
        public:
            std::vector<int> ports;
            vector<char*> methodName;
            
            MemCached(Analysis* analysis);
            
            bool isPacketOf(u_int16_t sport, u_int16_t dport);	/* If the packet carries HTTP(request or response) data */
            bool isHeaderPacket(const char *ptr, const int datalen);
            char* isRequest(const char *p, const int datalen);	    /* If the packet carries HTTP request data */
            char* isResponse(const char *p, const int datalen);	    /* If the packet carries HTTP response data */

            int extractPair(Flow* flow, bool closed);
            _protocol::Request* getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);
            _protocol::Response* getResponse(const char *data, const char *dataend, long ack);
            char* getMethodName(int m);
            int getMethodCount();
            int getStatusCount();
    };

    class Request : public _protocol::Request {
        public:
            
            Method method;
            u_int32_t   seq;
            u_int32_t   nxt_seq;
            vector<const char*> keys;
            char*       time;
            u_int32_t expiration;
            u_int8_t  flags; 

            Request();
            Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);   /* Parse the packet and store in a Request object */
            ~Request();
            
    };

    /*
    * Cache response header
    */
    class Response : public _protocol::Response {
        public:
            Status      status;
            long	acknowledgement;
            vector<const char*> keys;
            u_int8_t  flags;
            u_int32_t bodylen;
            Response();
            Response(const char *data, const char *dataend, long ack);	/* Parse the packet and store in a Response object */
            ~Response();
            
    };
}

using namespace _memcached;
#endif
