#include "http.hpp"

/*
 * From xplico.
 * Get HTTP request method by parsing header line.
 */
Method _http::parseMethod(const char *data, int linelen)
{
    const char *ptr;
    int	index = 0;

    /*
     * From RFC 2774 - An HTTP Extension Framework
     *
     * Support the command prefix that identifies the presence of
     * a "mandatory" header.
     */
    if (linelen >= 2) {
        if (strncmp(data, "M-", 2) == 0 || strncmp(data, "\r\n", 2) == 0) { /* \r\n necesary for bug in client POST */
            data += 2;
            linelen -= 2;
        }
    }

    /*
     * From draft-cohen-gena-client-01.txt, available from the uPnP forum:
     *	NOTIFY, SUBSCRIBE, UNSUBSCRIBE
     *
     * From draft-ietf-dasl-protocol-00.txt, a now vanished Microsoft draft:
     *	SEARCH
     */
    ptr = (const char *)data;
    /* Look for the space following the Method */
    while (index < linelen) {
        if (*ptr == ' ')
            break;
        else {
            ptr++;
            index++;
        }
    }

    /* Check the methods that have same length */
    switch (index) {
    case 3:
        if (strncmp(data, "GET", index) == 0) {
            return HTTP_MT_GET;
        }
        else if (strncmp(data, "PUT", index) == 0) {
            return HTTP_MT_PUT;
        }
#if 0
	else if (strncmp(data, "ICY", index) == 0) {
            return HTTP_MT_ICY;
        }
#endif
        break;

    case 4:
        if (strncmp(data, "COPY", index) == 0) {
            return HTTP_MT_COPY;
        }
        else if (strncmp(data, "HEAD", index) == 0) {
            return HTTP_MT_HEAD;
        }
        else if (strncmp(data, "LOCK", index) == 0) {
            return HTTP_MT_LOCK;
        }
        else if (strncmp(data, "MOVE", index) == 0) {
            return HTTP_MT_MOVE;
        }
        else if (strncmp(data, "POLL", index) == 0) {
            return HTTP_MT_POLL;
        }
        else if (strncmp(data, "POST", index) == 0) {
            return HTTP_MT_POST;
        }
        break;

    case 5:
        if (strncmp(data, "BCOPY", index) == 0) {
            return HTTP_MT_BCOPY;
        }
        else if (strncmp(data, "BMOVE", index) == 0) {
            return HTTP_MT_BMOVE;
        }
        else if (strncmp(data, "MKCOL", index) == 0) {
            return HTTP_MT_MKCOL;
        }
        else if (strncmp(data, "TRACE", index) == 0) {
            return HTTP_MT_TRACE;
        }
        else if (strncmp(data, "LABEL", index) == 0) {  /* RFC 3253 8.2 */
            return HTTP_MT_LABEL;
        }
        else if (strncmp(data, "MERGE", index) == 0) {  /* RFC 3253 11.2 */
            return HTTP_MT_MERGE;
        }
        break;

    case 6:
        if (strncmp(data, "DELETE", index) == 0) {
            return HTTP_MT_DELETE;
        }
        else if (strncmp(data, "SEARCH", index) == 0) {
            return HTTP_MT_SEARCH;
        }
        else if (strncmp(data, "UNLOCK", index) == 0) {
            return HTTP_MT_UNLOCK;
        }
        else if (strncmp(data, "REPORT", index) == 0) {  /* RFC 3253 3.6 */
            return HTTP_MT_REPORT;
        }
        else if (strncmp(data, "UPDATE", index) == 0) {  /* RFC 3253 7.1 */
            return HTTP_MT_UPDATE;
        }
        else if (strncmp(data, "NOTIFY", index) == 0) {
            return HTTP_MT_NOTIFY;
        }
        break;

    case 7:
        if (strncmp(data, "BDELETE", index) == 0) {
            return HTTP_MT_BDELETE;
        }
        else if (strncmp(data, "CONNECT", index) == 0) {
            return HTTP_MT_CONNECT;
        }
        else if (strncmp(data, "OPTIONS", index) == 0) {
            return HTTP_MT_OPTIONS;
        }
        else if (strncmp(data, "CHECKIN", index) == 0) {  /* RFC 3253 4.4, 9.4 */
            return HTTP_MT_CHECKIN;
        }
        break;

    case 8:
        if (strncmp(data, "PROPFIND", index) == 0) {
            return HTTP_MT_PROPFIND;
        }
        else if (strncmp(data, "CHECKOUT", index) == 0) { /* RFC 3253 4.3, 9.3 */
            return HTTP_MT_CHECKOUT;
        }
        /*
        else if (strncmp(data, "CCM_POST", index) == 0) {
            return HTTP_MT_CCM_POST;
        }
        */
        break;

    case 9:
        if (strncmp(data, "SUBSCRIBE", index) == 0) {
            return HTTP_MT_SUBSCRIBE;
        }
        else if (strncmp(data, "PROPPATCH", index) == 0) {
            return HTTP_MT_PROPPATCH;
        }
        else  if (strncmp(data, "BPROPFIND", index) == 0) {
            return HTTP_MT_BPROPFIND;
        }
        break;

    case 10:
        if (strncmp(data, "BPROPPATCH", index) == 0) {
            return HTTP_MT_BPROPPATCH;
        }
        else if (strncmp(data, "UNCHECKOUT", index) == 0) {  /* RFC 3253 4.5 */
            return HTTP_MT_UNCHECKOUT;
        }
        else if (strncmp(data, "MKACTIVITY", index) == 0) {  /* RFC 3253 13.5 */
            return HTTP_MT_MKACTIVITY;
        }
        break;

    case 11:
        if (strncmp(data, "MKWORKSPACE", index) == 0) {  /* RFC 3253 6.3 */
            return HTTP_MT_MKWORKSPACE;
        }
        else if (strncmp(data, "UNSUBSCRIBE", index) == 0) {
            return HTTP_MT_UNSUBSCRIBE;
        }
        /*
        else if (strncmp(data, "RPC_CONNECT", index) == 0) {
            return HTTP_MT_RPC_CONNECT;
        }
        */
        break;

    case 15:
        if (strncmp(data, "VERSION-CONTROL", index) == 0) {  /* RFC 3253 3.5 */
            return HTTP_MT_VERSION_CONTROL;
        }
        break;

    case 16:
        if (strncmp(data, "BASELINE-CONTROL", index) == 0) {  /* RFC 3253 12.6 */
            return HTTP_MT_BASELINE_CONTROL;
        }
        break;

    default:
        break;
    }

    return HTTP_MT_NONE;
}

/*
 * To identify if the packet is carrying HTTP request message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* Http::isRequest(const char *ptr, const int datalen) {
	Method method = HTTP_MT_NONE;
	char *head_end = NULL;

	method = parseMethod(ptr, datalen);
	if (method == HTTP_MT_NONE){
		return NULL;
	}
	else{
		int line_cnt = 0;
		head_end = Protocol::find_header_end(ptr, (ptr+datalen-1), &line_cnt);
		return head_end;
	}
}
/*
 * To identify if the packet is carrying HTTP response message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* Http::isResponse(const char *ptr, const int datalen) {
	Version version = HTTP_VER_NONE;
	char *head_end = NULL;

	if (datalen < 8){
		return NULL;
	}

	version = Response::parseVersion(ptr, datalen);
	if (version == HTTP_VER_NONE){
		return NULL;
	}
	else{
		int line_cnt = 0;
		head_end = Protocol::find_header_end(ptr, (ptr+datalen-1), &line_cnt);
		return head_end;
	}
}

Http::Http(Analysis* analysis) : Protocol(analysis) {
    //reqType = new int[statusCode.size()]{};
    ports.insert(ports.end(), {80, 8080, 8000});

    methodName = {
        "OPTIONS",
        "GET",
        "HEAD",
        "POST",
        "PUT",
        "DELETE",
        "TRACE",
        "CONNECT",
        "PATCH",
        "LINK",
        "UNLINK",
        "PROPFIND",
        "MKCOL",
        "COPY",
        "MOVE",
        "LOCK",
        "UNLOCK",
        "POLL",
        "BCOPY",
        "BMOVE",
        "SEARCH",
        "BDELETE",
        "PROPPATCH",
        "BPROPFIND",
        "BPROPPATCH",
        "LABEL",
        "MERGE",
        "REPORT",
        "UPDATE",
        "CHECKIN",
        "CHECKOUT",
        "UNCHECKOUT",
        "MKACTIVITY",
        "MKWORKSPACE",
        "VERSION-CONTROL",
        "BASELINE-CONTROL",
        "NOTIFY",
        "SUBSCRIBE",
        "UNSUBSCRIBE",
        "ICY",
        "NONE"
    };
}

bool Http::isPacketOf(u_int16_t sport, u_int16_t dport) {
    if(find(ports.begin(), ports.end(), sport) != ports.end()
    || find(ports.begin(), ports.end(), dport) != ports.end()) {
		return true;
	}
    return false;
}

bool Http::isHeaderPacket(const char *ptr, const int datalen) {
    char *req_head_end = NULL;
	char *rsp_head_end = NULL;

	req_head_end = isRequest(ptr, datalen);
	rsp_head_end = isResponse(ptr, datalen);

	if ( (req_head_end != NULL) || (rsp_head_end != NULL)){
		return true;
	}

	return false;
}

_http::Request::~Request() {
	if(host != NULL)
		free(host);
	if(uri != NULL)
		free(uri);
	if(user_agent != NULL)
		free(user_agent);
	if(referer != NULL)
		free(referer);
	if(connection != NULL)
	    free(connection);
    if(accept != NULL)
        free(accept);
    if(accept_encoding != NULL)
        free(accept_encoding);
	if(accept_language != NULL)
        free(accept_language);
    if(accept_charset != NULL)
        free(accept_charset);
    if(cookie != NULL)
        free(cookie);
	if(content_type != NULL)
		free(content_type);
	if(content_encoding != NULL)
		free(content_encoding);
	if(content_length != NULL)
        free(content_length);
    if(time != NULL)
        free(time);
}

_http::Response::~Response() {
	if(server != NULL)
		free(server);
	if(date != NULL)
		free(date);
	if(expires != NULL)
		free(expires);
	if(location != NULL)
		free(location);
	if(etag != NULL)
		free(etag);
	if(accept_ranges != NULL)
		free(accept_ranges);
	if(last_modified != NULL)
		free(last_modified);
	if(content_type != NULL)
		free(content_type);
	if(content_encoding != NULL)
		free(content_encoding);
	if(content_length != NULL)
        free(content_length);
    if(age != NULL)
        free(age);
}

/*
 * From xplico.
 * Get HTTP request URI by parsing header line.
 * Return NULL if no URI found.
 */
char* _http::parseUri(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    int tokenlen;
    char *uri;

    lineend = line + len;

    /* The first token is the method. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ') {
        return NULL;
    }
    line = next_token;

    /* The next token is the URI. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ')
        return NULL;

    uri = MALLOC(char, tokenlen+1);
    if (uri != NULL) {
        memcpy(uri, line, tokenlen);
        uri[tokenlen] = '\0';
    }

    return uri;
}

/*
 * From xplico.
 * Get HTTP request version by parsing header line.
 */
Version _http::Request::parseVersion(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    int tokenlen;

    lineend = line + len;

    /* The first token is the method. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ') {
        return HTTP_VER_NONE;
    }
    line = next_token;

    /* The next token is the URI. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ')
        return HTTP_VER_NONE;
    line = next_token;

    /* Everything to the end of the line is the version. */
    tokenlen = lineend - line;
    if (tokenlen == 0)
        return HTTP_VER_NONE;

    if (strncmp(line, "HTTP/1.0", 8) == 0)
        return HTTP_VER_1_0;

    if (strncmp(line, "HTTP/1.1", 8) == 0)
        return HTTP_VER_1_1;

    return HTTP_VER_NONE;
}


/*
 * From xplico.
 * Get HTTP response version by parsing header line.
 */
Version _http::Response::parseVersion(const char *line, int len) {
    if (strncmp(line, "HTTP/1.0", 8) == 0)
        return HTTP_VER_1_0;

    if (strncmp(line, "HTTP/1.1", 8) == 0)
        return HTTP_VER_1_1;

    return HTTP_VER_NONE;
}

/*
 * From xplico.
 * Get the HTTP response status code by parsing header line.
 */
Status _http::parseStatus(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    Status status;
    int tokenlen, val;
    
    lineend = line + len;
    status = HTTP_ST_NONE;

    /* The first token is the protocol and version */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ') {
        return status;
    }

    line = next_token;
    /* The next token is status value. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || (line[tokenlen] != ' ' && line[tokenlen] != '\r' && line[tokenlen] != '\n')) {
        return status;
    }

    /*
     * Parse response status value.
     */
    if (sscanf(line, "%i", &val) != 1) {
		return status;
	}

    /* search enum */
    map<int, Status>::iterator it;
    it = statusCode.find(val);
    if (it != statusCode.end()) status = it->second;

    return status;
}

_protocol::Request* Http::getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) {
    return new Request(data, dataend, time, seq, nxt_seq);
}

_protocol::Response* Http::getResponse(const char *data, const char *dataend, long ack) {
    return new Response(data, dataend, ack);
}

char* Http::getMethodName(int m) {
    return methodName[m];
}

int Http::getMethodCount() {
    return static_cast<int>(methodName.size());
}

int Http::getStatusCount() {
    return static_cast<int>(statusCode.size());
}

/*
 * Extract request message from data.
 * But only the header fields are extracted.
 */

_http::Request::Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq) {
	char *eoh, *eol, *linesp, *lineep;
	int line_cnt = 0, lnl = 0, hdl = 0;

    seq = seq;
    nxt_seq = nxt_seq;

	eoh = Protocol::find_header_end(data, dataend, &line_cnt);
	hdl = eoh - data + 1;
	hdlen = hdl;

	/** Added Functionality: Store time and sequence + tcp length
	 *  in request_header
	**/
	time = strdup(time);
	/* Parse first line of http request */
	linesp = (char*) data;
	lineep = Protocol::find_line_end(linesp, eoh, &eol);
	lnl = lineep - linesp + 1;
	method = parseMethod(linesp, lnl);
	if ( method == HTTP_MT_NONE){
        return;
	}

	uri = parseUri(linesp, lnl);
    version = parseVersion(linesp, lnl);
	host = Protocol::header_param(data, hdl, "Host:");
	referer = Protocol::header_param(data, hdl, "Referer:");
	user_agent = Protocol::header_param(data, hdl, "User-Agent:");
	connection = Protocol::header_param(data, hdl, "Connection:");
	accept = Protocol::header_param(data, hdl, "Accept:");
	accept_encoding = Protocol::header_param(data, hdl, "Accept-Encoding:");
	accept_language = Protocol::header_param(data, hdl, "Accept-Language:");
	accept_charset = Protocol::header_param(data, hdl, "Accept-Charset:");
	cookie = Protocol::header_param(data, hdl, "Cookie:");

	if (method == HTTP_MT_POST ){
		content_type = Protocol::header_param(data, hdl, "Content-Type:");
		content_encoding = Protocol::header_param(data, hdl, "Content-Encoding:");
		content_length = Protocol::header_param(data, hdl, "Content-Length:");
	}
}

_http::Response::Response() {
    statusCode[100] = HTTP_ST_100;
    statusCode[101] = HTTP_ST_101;
    statusCode[102] = HTTP_ST_102;
    statusCode[199] = HTTP_ST_199;

    statusCode[200] = HTTP_ST_200;
    statusCode[201] = HTTP_ST_201;
    statusCode[202] = HTTP_ST_202;
    statusCode[203] = HTTP_ST_203;
    statusCode[204] = HTTP_ST_204;
    statusCode[205] = HTTP_ST_205;
    statusCode[206] = HTTP_ST_206;
    statusCode[207] = HTTP_ST_207;
    statusCode[299] = HTTP_ST_299;

    statusCode[300] = HTTP_ST_300;
    statusCode[301] = HTTP_ST_301;
    statusCode[302] = HTTP_ST_302;
    statusCode[303] = HTTP_ST_303;
    statusCode[304] = HTTP_ST_304;
    statusCode[305] = HTTP_ST_305;
    statusCode[307] = HTTP_ST_307;
    statusCode[399] = HTTP_ST_399;

    statusCode[400] = HTTP_ST_400;
    statusCode[401] = HTTP_ST_401;
    statusCode[402] = HTTP_ST_402;
    statusCode[403] = HTTP_ST_403;
    statusCode[404] = HTTP_ST_404;
    statusCode[405] = HTTP_ST_405;
    statusCode[406] = HTTP_ST_406;
    statusCode[407] = HTTP_ST_407;
    statusCode[408] = HTTP_ST_408;
    statusCode[409] = HTTP_ST_409;
    statusCode[410] = HTTP_ST_410;
    statusCode[411] = HTTP_ST_411;
    statusCode[412] = HTTP_ST_412;
    statusCode[413] = HTTP_ST_413;
    statusCode[414] = HTTP_ST_414;
    statusCode[415] = HTTP_ST_415;
    statusCode[416] = HTTP_ST_416;
    statusCode[417] = HTTP_ST_417;
    statusCode[422] = HTTP_ST_422;
    statusCode[423] = HTTP_ST_423;
    statusCode[424] = HTTP_ST_424;
    statusCode[499] = HTTP_ST_499;

    statusCode[500] = HTTP_ST_500;
    statusCode[501] = HTTP_ST_501;
    statusCode[502] = HTTP_ST_502;
    statusCode[503] = HTTP_ST_503;
    statusCode[504] = HTTP_ST_504;
    statusCode[505] = HTTP_ST_505;
    statusCode[507] = HTTP_ST_507;
    statusCode[599] = HTTP_ST_599;
}

/*
 * Extract response message from data.
 * But only the header fields are extracted.
 */
_http::Response::Response(const char *data, const char *dataend, long ack) : Response() {
	char *eoh, *eol, *linesp, *lineep;
	int line_cnt = 0, lnl = 0, hdl = 0;
	
	eoh = Protocol::find_header_end(data, dataend, &line_cnt);
	hdl = eoh -data + 1;
	hdlen = hdl;
	
	/** Added Functionality: Store acknowledgement number
	 *  in response_header
	**/
	acknowledgement = ack;

	/* first line */
	linesp = (char*) data;
	lineep = Protocol::find_line_end(linesp, eoh, &eol);
	lnl = lineep - linesp + 1;

	status = parseStatus(linesp, lnl);
	if (status == HTTP_ST_NONE){
		return;
	}

	version = parseVersion(linesp, lnl);
	server = Protocol::header_param(data, hdl, "Server:");

	/* Cache */
	date = Protocol::header_param(data, hdl, "Date:");
	expires = Protocol::header_param(data, hdl, "Expires:");
	last_modified = Protocol::header_param(data, hdl, "Last-Modified:");
	etag = Protocol::header_param(data, hdl, "ETag:");
	age = Protocol::header_param(data, hdl, "Age:");
	
	/* Content */
	accept_ranges = Protocol::header_param(data, hdl, "Accept-Ranges:");
	content_type = Protocol::header_param(data, hdl, "Content-Type:");
	content_encoding = Protocol::header_param(data, hdl, "Content-Encoding:");
	content_length = Protocol::header_param(data, hdl, "Content-Length:");
	location = Protocol::header_param(data, hdl, "Location:");
}