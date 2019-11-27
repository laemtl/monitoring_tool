#ifndef __HTTP_H__
#define __HTTP_H__

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

namespace _http {
    enum Status {
        HTTP_ST_100=100,   /**< Continue */
        HTTP_ST_101,       /**< Switching Protocols */
        HTTP_ST_102,       /**< Processing */
        HTTP_ST_199=199,   /**< Informational - Others */

        HTTP_ST_200,       /**< OK */
        HTTP_ST_201,       /**< Created */
        HTTP_ST_202,       /**< Accepted */
        HTTP_ST_203,       /**< Non-authoritative Information */
        HTTP_ST_204,       /**< No Content */
        HTTP_ST_205,       /**< Reset Content */
        HTTP_ST_206,       /**< Partial Content */
        HTTP_ST_207,       /**< Multi-Status */
        HTTP_ST_299=299,   /**< Success - Others */

        HTTP_ST_300,       /**< Multiple Choices */
        HTTP_ST_301,       /**< Moved Permanently */
        HTTP_ST_302,       /**< Found */
        HTTP_ST_303,       /**< See Other */
        HTTP_ST_304,       /**< Not Modified */
        HTTP_ST_305,       /**< Use Proxy */
        HTTP_ST_307,       /**< Temporary Redirect */
        HTTP_ST_399=399,   /**< Redirection - Others */

        HTTP_ST_400,       /**< Bad Request */
        HTTP_ST_401,       /**< Unauthorized */
        HTTP_ST_402,       /**< Payment Required */
        HTTP_ST_403,       /**< Forbidden */
        HTTP_ST_404,       /**< Not Found */
        HTTP_ST_405,       /**< Method Not Allowed */
        HTTP_ST_406,       /**< Not Acceptable */
        HTTP_ST_407,       /**< Proxy Authentication Required */
        HTTP_ST_408,       /**< Request Time-out */
        HTTP_ST_409,       /**< Conflict */
        HTTP_ST_410,       /**< Gone */
        HTTP_ST_411,       /**< Length Required */
        HTTP_ST_412,       /**< Precondition Failed */
        HTTP_ST_413,       /**< Request Entity Too Large */
        HTTP_ST_414,       /**< Request-URI Too Long */
        HTTP_ST_415,       /**< Unsupported Media Type */
        HTTP_ST_416,       /**< Requested Range Not Satisfiable */
        HTTP_ST_417,       /**< Expectation Failed */
        HTTP_ST_422=422,   /**< Unprocessable Entity */
        HTTP_ST_423,       /**< Locked */
        HTTP_ST_424,       /**< Failed Dependency */
        HTTP_ST_499=499,   /**< Client Error - Others */

        HTTP_ST_500,       /**< Internal Server Error */
        HTTP_ST_501,       /**< Not Implemented */
        HTTP_ST_502,       /**< Bad Gateway */
        HTTP_ST_503,       /**< Service Unavailable */
        HTTP_ST_504,       /**< Gateway Time-out */
        HTTP_ST_505,       /**< HTTP Version not supported */
        HTTP_ST_507=507,   /**< Insufficient Storage */
        HTTP_ST_599=599,   /**< Server Error - Others */
        HTTP_ST_NONE
    };
    static std::map<int, Status> statusCode;

    enum Method {
        HTTP_MT_OPTIONS = 0, /* RFC2616 */
        HTTP_MT_GET,
        HTTP_MT_HEAD,
        HTTP_MT_POST,
        HTTP_MT_PUT,
        HTTP_MT_DELETE,
        HTTP_MT_TRACE,
        HTTP_MT_CONNECT,
        HTTP_MT_PATCH,
        HTTP_MT_LINK,
        HTTP_MT_UNLINK,
        HTTP_MT_PROPFIND,    /* RFC2518 */
        HTTP_MT_MKCOL,
        HTTP_MT_COPY,
        HTTP_MT_MOVE,
        HTTP_MT_LOCK,
        HTTP_MT_UNLOCK,
        HTTP_MT_POLL,        /* Outlook Web Access */
        HTTP_MT_BCOPY,
        HTTP_MT_BMOVE,
        HTTP_MT_SEARCH,
        HTTP_MT_BDELETE,
        HTTP_MT_PROPPATCH,
        HTTP_MT_BPROPFIND,
        HTTP_MT_BPROPPATCH,
        HTTP_MT_LABEL,             /* RFC 3253 8.2 */
        HTTP_MT_MERGE,             /* RFC 3253 11.2 */
        HTTP_MT_REPORT,            /* RFC 3253 3.6 */
        HTTP_MT_UPDATE,            /* RFC 3253 7.1 */
        HTTP_MT_CHECKIN,           /* RFC 3253 4.4, 9.4 */
        HTTP_MT_CHECKOUT,          /* RFC 3253 4.3, 9.3 */
        HTTP_MT_UNCHECKOUT,        /* RFC 3253 4.5 */
        HTTP_MT_MKACTIVITY,        /* RFC 3253 13.5 */
        HTTP_MT_MKWORKSPACE,       /* RFC 3253 6.3 */
        HTTP_MT_VERSION_CONTROL,   /* RFC 3253 3.5 */
        HTTP_MT_BASELINE_CONTROL,  /* RFC 3253 12.6 */
        HTTP_MT_NOTIFY,            /* uPnP forum */
        HTTP_MT_SUBSCRIBE,
        HTTP_MT_UNSUBSCRIBE,
        HTTP_MT_ICY,               /* Shoutcast client (forse) */
        HTTP_MT_NONE
    };
    
    enum Version {
        HTTP_VER_1_0,
        HTTP_VER_1_1,
        HTTP_VER_NONE
    };

    Status parseStatus(const char *line, int len);
    Method parseMethod(const char *data, int linelen);
    char* parseUri(const char *line, int len);

    class Http : public _protocol::Protocol {
        public:
            std::vector<int> ports;
            vector<char*> methodName;
            
            Http(Analysis* analysis);
            //Http();
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
            Version 	 version;
            Method	     method;
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
            /* HTTP_MT_POST */
            char*		 content_type;
            char*		 content_encoding;
            char*		 content_length;

            u_int32_t   seq;
            u_int32_t   nxt_seq;

            Request();
            Request(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);   /* Parse the packet and store in a Request object */
            ~Request();
            static Version parseVersion(const char *line, int len);
    };

    /*
    * HTTP response header
    */
    class Response : public _protocol::Response {
        public:
            Status       	status;
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
    };
}

using namespace _http;

#endif