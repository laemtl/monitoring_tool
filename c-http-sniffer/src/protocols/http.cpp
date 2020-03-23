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
    ports.insert(ports.end(), {80, 8080, 8000});
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

    /* search enum */
    vector<int>::iterator it;
    it = find(status.begin(), status.end(), val);
    if (it != status.end()) statusCode = distance(status.begin(), it);
    
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
    status[0];
    status[100];
    status[101];
    status[102];
    status[199];

    status[200];
    status[201];
    status[202];
    status[203];
    status[204];
    status[205];
    status[206];
    status[207];
    status[299];

    status[300];
    status[301];
    status[302];
    status[303];
    status[304];
    status[305];
    status[307];
    status[399];

    status[400];
    status[401];
    status[402];
    status[403];
    status[404];
    status[405];
    status[406];
    status[407];
    status[408];
    status[409];
    status[410];
    status[411];
    status[412];
    status[413];
    status[414];
    status[415];
    status[416];
    status[417];
    status[422];
    status[423];
    status[424];
    status[499];

    status[500];
    status[501];
    status[502];
    status[503];
    status[504];
    status[505];
    status[507];
    status[599];
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
