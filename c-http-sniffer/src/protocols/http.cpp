#include "http.hpp"

#include <iostream>

/*
 * From xplico.
 * Get HTTP request method by parsing header line.
 */
int _http::Request::parseMethod(const char *data, int linelen)
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

    for (std::size_t i = 0; i != methodsName.size(); ++i) {
        if (strcmp(data, methodsName[i]) == 0) {
            return i;
        }
        break;
    }

    return 0;
}

/*
 * To identify if the packet is carrying HTTP request message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* Http::isRequest(const char *ptr, const int datalen) {
	int methodCode = 0;
	char *head_end = NULL;

    _http::Request* req = new _http::Request();
	methodCode = req->parseMethod(ptr, datalen);
	if (methodCode == 0){
		return NULL;
	} else {
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
	} else {
		int line_cnt = 0;
		head_end = Protocol::find_header_end(ptr, (ptr+datalen-1), &line_cnt);
		return head_end;
	}
}

Http::Http(Analysis* analysis) : Protocol(analysis) {
    serverPorts.insert(serverPorts.end(), {80, 8080, 8000});

    metrics.push_back(new Rst(this, analysis));
    metrics.push_back(new ReqRate(this, analysis));
    metrics.push_back(new ErrRate(this, analysis));
    metrics.push_back(new Tp(this, analysis));
    metrics.push_back(new TpRev(this, analysis));
    metrics.push_back(new ConnRate(this, analysis));

    metrics.push_back(new Client(this, analysis));
    metrics.push_back(new ReqPath(this, analysis));
    metrics.push_back(new ReqMethod(this, analysis));
    metrics.push_back(new ReqType(this, analysis));
    metrics.push_back(new RspStatus(this, analysis));
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


bool _http::Response::hasErrorStatus() {
    // Extract first digit of status code
	int i = statusCode;
	while (i>=10) i=i/10;  
	if (i>3) {
		return true;
	}
    return false;
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
int _http::Response::parseStatus(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    int statusCode;
    int tokenlen, val;
    
    lineend = line + len;
    statusCode = 0;

    /* The first token is the protocol and version */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || line[tokenlen] != ' ') {
        return statusCode;
    }

    line = next_token;
    /* The next token is status value. */
    tokenlen = Protocol::get_token_len(line, lineend, &next_token);
    if (tokenlen == 0 || (line[tokenlen] != ' ' && line[tokenlen] != '\r' && line[tokenlen] != '\n')) {
        return statusCode;
    }

    /*
     * Parse response status value.
     */
    if (sscanf(line, "%i", &val) != 1) {
		return statusCode;
	}

    for (auto &s : status) {
        if (s.first == val) {
            statusCode = s.first;
            break;
        }
    }
    
    return statusCode;
}

_protocol::Request* Http::getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) {
    return new Request(data, dataend, time, seq, nxt_seq);
}

_protocol::Response* Http::getResponse(const char *data, const char *dataend, long ack) {
    return new Response(data, dataend, ack);
}

_http::Request::Request() {
    methodsName.push_back("OPTIONS");           /* RFC2616 */
    methodsName.push_back("GET");
    methodsName.push_back("HEAD");
    methodsName.push_back("POST");
    methodsName.push_back("PUT");
    methodsName.push_back("DELETE");
    methodsName.push_back("TRACE");
    methodsName.push_back("CONNECT");
    methodsName.push_back("PATCH");
    methodsName.push_back("LINK");
    methodsName.push_back("UNLINK");
    methodsName.push_back("PROPFIND");          /* RFC2518 */
    methodsName.push_back("MKCOL");
    methodsName.push_back("COPY");
    methodsName.push_back("MOVE");
    methodsName.push_back("LOCK");
    methodsName.push_back("UNLOCK");
    methodsName.push_back("POLL");              /* Outlook Web Access */
    methodsName.push_back("BCOPY");
    methodsName.push_back("BMOVE");
    methodsName.push_back("SEARCH");
    methodsName.push_back("BDELETE");
    methodsName.push_back("PROPPATCH");
    methodsName.push_back("BPROPFIND");
    methodsName.push_back("BPROPPATCH");
    methodsName.push_back("LABEL");             /* RFC 3253 8.2 */
    methodsName.push_back("MERGE");             /* RFC 3253 11.2 */
    methodsName.push_back("REPORT");            /* RFC 3253 3.6 */
    methodsName.push_back("UPDATE");            /* RFC 3253 7.1 */
    methodsName.push_back("CHECKIN");           /* RFC 3253 4.4"); 9.4 */
    methodsName.push_back("CHECKOUT");          /* RFC 3253 4.3"); 9.3 */
    methodsName.push_back("UNCHECKOUT");        /* RFC 3253 4.5 */
    methodsName.push_back("MKACTIVITY");        /* RFC 3253 13.5 */
    methodsName.push_back("MKWORKSPACE");       /* RFC 3253 6.3 */
    methodsName.push_back("VERSION_CONTROL");   /* RFC 3253 3.5 */
    methodsName.push_back("BASELINE_CONTROL");  /* RFC 3253 12.6 */
    methodsName.push_back("NOTIFY");            /* uPnP forum */
    methodsName.push_back("SUBSCRIBE");
    methodsName.push_back("UNSUBSCRIBE");
    methodsName.push_back("ICY");               /* Shoutcast client (forse) */
    methodsName.push_back("NONE");
}

/*
 * Extract request message from data.
 * But only the header fields are extracted.
 */

_http::Request::Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq) : Request() {
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
	methodCode = parseMethod(linesp, lnl);
	if (methodsName[methodCode] == "NONE"){
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

	if (methodsName[methodCode] == "POST"){
		content_type = Protocol::header_param(data, hdl, "Content-Type:");
		content_encoding = Protocol::header_param(data, hdl, "Content-Encoding:");
		content_length = Protocol::header_param(data, hdl, "Content-Length:");
	}
}

_http::Response::Response() {
    status.insert({0, "None"});
    status.insert({100, "100 Continue"});
    status.insert({101, "101 Switching Protocols"});
    status.insert({102, "102 Processing"});
    status.insert({199, "199 Miscellaneous Warning"});

    status.insert({200, "200 OK"});
    status.insert({201, "201 Created"});
    status.insert({202, "202 Accepted"});
    status.insert({203, "203 Non-Authoritative Information"});
    status.insert({204, "204 No Content"});
    status.insert({205, "205 Reset Content"});
    status.insert({206, "206 Partial Content"});
    status.insert({207, "207 Multi-Status"});
    status.insert({299, "299 Miscellaneous Persistent Warning"});

    status.insert({300, "300 Multiple Choices"});
    status.insert({301, "301 Moved Permanently"});
    status.insert({302, "302 Found"});
    status.insert({303, "303 See Other"});
    status.insert({304, "304 Not Modified"});
    status.insert({305, "305 Use Proxy"});
    status.insert({307, "307 Temporary Redirect"});
    status.insert({399, "399"});

    status.insert({400, "400 Bad Request"});
    status.insert({401, "401 Unauthorized"});
    status.insert({402, "402 Payment Required"});
    status.insert({403, "403 Forbidden"});
    status.insert({404, "404 Not Found"});
    status.insert({405, "405 Method Not Allowed"});
    status.insert({406, "406 Not Acceptable"});
    status.insert({407, "407 Proxy Authentication Required"});
    status.insert({408, "408 Request Timeout"});
    status.insert({409, "409 Conflict"});
    status.insert({410, "410 Gone"});
    status.insert({411, "411 Length Required"});
    status.insert({412, "412 Precondition Failed"});
    status.insert({413, "413 Payload Too Large"});
    status.insert({414, "414 URI Too Long"});
    status.insert({415, "415 Unsupported Media Type"});
    status.insert({416, "416 Range Not Satisfiable"});
    status.insert({417, "417 Expectation Failed"});
    status.insert({422, "422 Unprocessable Entity"});
    status.insert({423, "423 Locked"});
    status.insert({424, "424 Failed Dependency"});
    status.insert({499, "499 Client Closed Request"});

    status.insert({500, "500 Internal Server Error"});
    status.insert({501, "501 Not Implemented"});
    status.insert({502, "502 Bad Gateway"});
    status.insert({503, "503 Service Unavailable"});
    status.insert({504, "504 Gateway Timeout"});
    status.insert({505, "505 HTTP Version Not Supported"});
    status.insert({507, "507 Insufficient Storage"});
    status.insert({599, "599 Network Connect Timeout Error"});
}

/*
 * Extract response message from data.
 * But only the header fields are extracted.
 */
_http::Response::Response(const char *data, const char *dataend, long ack) {
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

	statusCode = parseStatus(linesp, lnl);
	if (statusCode == 0){
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
