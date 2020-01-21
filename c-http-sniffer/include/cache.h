/*
 * cache.h
 *
 *  Created on: Nov 22,2019
 *      Author: MonaEssam
 */
#ifndef __cache_H__
#define __cache_H__

#include <sys/types.h>
#include <time.h>

#include "packet.h"
#include "util.h"


enum _cache_status
{
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
VERSION};
typedef enum _cache_status cache_status;
extern cache_status cache_STATUS_ARRAY[]; 
enum _cache_mthd
{
get,
set,
add,
replace,
append,
prepend,
delete,
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
CACHE_ST_NONE
};
typedef enum _cache_mthd cache_mthd;

extern char *cache_METHOD_STRING_ARRAY[];    
typedef struct _request_t request_t;
struct _request_t
{
        cache_mthd       method;
        char*           key;
        char*           time;
        uint32_t expiration;
        uint8_t  flags;
        uint32_t bodylen;
       // int hdlen;  // Header length
    u_int32_t   seq;
 u_int32_t   nxt_seq;
};

/*
 * HTTP response header
 */
typedef struct _response_t response_t;
struct _response_t
{
        cache_status     status;
        char                    time[256];
        char                    size[256];
        long                    acknowledgement;
        int                             hdlen;  // Header length
};

/*
 * HTTP request and response pair.
 */
typedef struct _pair_t  pair_t;
struct _pair_t
{
        request_t       *request_header;
        response_t      *response_header;
        time_t  req_fb_sec;
        time_t  req_fb_usec;
        time_t  req_lb_sec;
        time_t  req_lb_usec;
        time_t  rsp_fb_sec;
        time_t  rsp_fb_usec;
        time_t  rsp_lb_sec;
        time_t  rsp_lb_usec;
        u_int32_t       req_total_len;
        u_int32_t       rsp_total_len;
        u_int32_t       req_body_len;
        u_int32_t       rsp_body_len;

        pair_t  *next;
    BOOL req_processed;
    BOOL rsp_processed;                                     /* if the http_pair has already been processed or not */
};

char* IsRequest(const char *p, const int datalen);          /* If the packet carries HTTP request data */
char* IsResponse(const char *p, const int datalen);         /* If the packet carries HTTP response data */
BOOL IsCachePacket(const char *p, const int datalen);    /* If the packet carries HTTP(request or response) data */

pair_t* cache_new(void);                                             /* Create a new pair_t object */
void pair_free(pair_t *h);                                      /* Free a pair_t object */
request_t* cache_request_new(void);                                      /* Create a new request_t object */

void http_request_free(request_t *req);                 /* Free a request_t object */
response_t* cache_response_new(void);                            /* Create a new response_t object */
void cache_response_free(response_t *rsp);                           /* Free a response_t object */
int cache_add_request(pair_t *h, request_t *req);        /* Add a request_t object to pair_t request chain */
int cache_add_response(pair_t *h, response_t *rsp);      /* Add a response_t object to pair_t response chain */

//int http_parse_request(request_t *request, const char *data, const char *dataend,char* time, long bytes); 
int cache_parse_request(request_t *request, const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq);  
int cache_parse_response(response_t *response, const char *data, const char *dataend, long ack); /* Parse the packet and store in a response_t object */

#endif /* __cache_H__ */
