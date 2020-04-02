#ifndef __HTTP_H__
#define __HTTP_H__

#include <sys/types.h>
#include <time.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <map>
#include "requestMethod.hpp"
#include "responseStatus.hpp"
#include "protocol.hpp"

namespace _http { 
    
    enum Version {
        HTTP_VER_1_0,
        HTTP_VER_1_1,
        HTTP_VER_NONE
    };

    char* parseUri(const char *line, int len);

    class Http : public _protocol::Protocol {
        public:
            Http(Analysis* analysis);
            bool isHeaderPacket(const char *ptr, const int datalen);
            char* isRequest(const char *p, const int datalen);	    /* If the packet carries HTTP request data */
            char* isResponse(const char *p, const int datalen);	    /* If the packet carries HTTP response data */

            int extractPair(Flow* flow, bool closed);
            _protocol::Request* getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);
            _protocol::Response* getResponse(const char *data, const char *dataend, long ack);
    };
    
    class Request : public _protocol::Request, RequestMethod {
        public:
            Version 	 version;
            char*		 host;
            char*		 uri;
            char*		 user_agent;
            char*		 referer;
            char*		 connection;
            char*		 accept;
            char*	 	 accept_encoding;
            char*		 accept_language;
            char*		 accept_charset;
            char*		 cookie;
            /* POST */
            char*		 content_type;
            char*		 content_encoding;
            char*		 content_length;

            u_int32_t   seq;
            u_int32_t   nxt_seq;

            Request();
            Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);   /* Parse the packet and store in a Request object */
            ~Request();
            static Version parseVersion(const char *line, int len);
            int parseMethod(const char *data, int linelen);
    };
    
    /*
    * HTTP response header
    */
    class Response : public _protocol::Response, ResponseStatus {
        public:
            Version	        version;
            char*			server;
            char*			date;
            char*			expires;
            char*			location;
            char*			etag;
            char*			accept_ranges;
            char*			last_modified;
            char*			content_type;
            char*			content_encoding;
            char* 			content_length;
            char*		 	age;
            long			acknowledgement;

            Response();
            Response(const char *data, const char *dataend, long ack);	/* Parse the packet and store in a Response object */
            ~Response();
            static Version parseVersion(const char *line, int len);
            virtual int parseStatus(const char *line, int len);
            virtual bool hasErrorStatus();
    };
}

#include "packet.h"
#include "util.h"

using namespace _http;

#endif