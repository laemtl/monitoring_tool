#include "memcached.hpp"

MemCached::MemCached(Analysis* analysis, char* protocolName, uint32_t protocolId) : Protocol(analysis, protocolName, protocolId) {
    serverPorts.push_back(11211);
    
    metrics.push_back(new Rst(this, analysis));
    metrics.push_back(new ErrRate(this, analysis));
    metrics.push_back(new Tp(this, analysis));
    metrics.push_back(new TpRev(this, analysis));
    metrics.push_back(new ReqType(this, analysis));
    metrics.push_back(new RspStatus(this, analysis));
}

/*
 * From xplico.
 * Get Cache request method by parsing header line.
 */
int _memcached::Request::parseMethod(const char *data, int linelen)
{
    const char *ptr;
    int	index = 0;
    ptr = (const char *)data;
    /* Look for the space following the Method */
    while (index < linelen) {
        if ((*ptr == ' ') || (*ptr == '\r'))
            break;
        else {
            ptr++;
            index++;
        }
    }

    for (std::size_t i = 0; i != methodsName.size(); ++i) {
        if (strncmp(data, methodsName[i], index) == 0) {
            return i;
        }
    }

    return 0;
}


/*
 * To identify if the packet is carrying Cache request message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* MemCached::isRequest(const char *ptr, const int datalen) {
    int methodCode = 0;
	char *eol,*linesp,*head_end = NULL;
    int lnl;
    
    linesp = (char*) ptr;
	head_end = Protocol::find_line_end(linesp, datalen, &eol);
	lnl = head_end - linesp + 1;
	
    _memcached::Request* req = new _memcached::Request();
    methodCode = req->parseMethod(linesp, lnl);
	if (methodCode == 0){
        return NULL;
	} else {
        return head_end;
	}
}

int _memcached::Response::parseStatus(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    const char *st;
    int statusCode;
    int tokenlen;
    
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
    st = MALLOC(char, tokenlen+1);
    if (st != NULL) {
        memcpy(st, line, tokenlen);
        //st[tokenlen] = '\0';
    }
    
    for (auto &s : status) {
        if (s.second == st) {
            statusCode = s.first;
            break;
        }
    }
    free(st);

    return statusCode;
}

bool _memcached::Response::hasErrorStatus() {
    if(status[statusCode] == "ERROR"
    || status[statusCode] == "CLIENT_ERROR"
    || status[statusCode] == "SERVER_ERROR"
    || status[statusCode] == "BADCLASS") return true;

    return false;
}

/*
 * To identify if the packet is carrying cache response message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* MemCached::isResponse(const char *ptr, const int datalen) {
	char *eol, *linesp, *head_end = NULL;
    int lnl;
    int statusCode = 0;

    linesp = (char*) ptr;
	head_end = Protocol::find_line_end(linesp, datalen, &eol);
	lnl = head_end - linesp + 1;

    _memcached::Response* rsp = new _memcached::Response();
	statusCode = rsp->parseStatus(linesp, lnl);
    
    if (rsp->status[statusCode] == "NONE"){
        return NULL;
	} else {
        return head_end;
	}
}

bool MemCached::isHeaderPacket(const char *ptr, const int datalen) {
    char *req_head_end = NULL;
	char *rsp_head_end = NULL;

	req_head_end = isRequest(ptr, datalen);
	rsp_head_end = isResponse(ptr, datalen);

	if ( (req_head_end != NULL) || (rsp_head_end != NULL)){
		return true;
	}

	return false;
}

_memcached::Request::~Request() {
    if (!keys.empty())
        std::vector<const char*>(keys).swap(keys);
    if(time != NULL)
        free(time);
}

_memcached::Response::~Response() {
     if (!keys.empty())
        std::vector<const char*>(keys).swap(keys);
}


_protocol::Request* MemCached::getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) {
    return new Request(data, dataend, time, seq, nxt_seq);
}

_protocol::Response* MemCached::getResponse(const char *data, const char *dataend, long ack) {
    return new Response(data, dataend, ack);
}

_memcached::Request::Request() {
    methodsName.push_back("none");
    methodsName.push_back("get");
    methodsName.push_back("set");
    methodsName.push_back("add");
    methodsName.push_back("replace");
    methodsName.push_back("append");
    methodsName.push_back("prepend");
    methodsName.push_back("delete");
    methodsName.push_back("flush_all");
    methodsName.push_back("quit");
    methodsName.push_back("incr");
    methodsName.push_back("decr");
    methodsName.push_back("touch");
    methodsName.push_back("gat");
    methodsName.push_back("gats");
    methodsName.push_back("slabs");
    methodsName.push_back("lru");
    methodsName.push_back("watch");
    methodsName.push_back("stats");
    methodsName.push_back("version");
    methodsName.push_back("misbehave");
}

/*
 * Extract request message from data.
 * But only the header fields are extracted.
 */

_memcached::Request::Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq) : Request() {
	char *eoh, *eol, *linesp, *lineep;
	int tokenlen = 0, hdl = 0;
    char *next_token, *key;
    unsigned int val;

    seq = seq;
    nxt_seq = nxt_seq;

	eoh = Protocol::find_line_end(data, dataend, &eol);
	hdl = eoh - data + 1;
	hdlen = hdl;
	time = strdup(time);
	/* Parse first line of cache request */
	linesp = (char*) data;
	methodCode = parseMethod(linesp, hdl);
	if (methodCode == 0){
        return;
	}
    
    if(methodsName[methodCode] == "get" /*|| method == gets*/) {
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        while (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
            key = MALLOC(char, tokenlen+1);
            if (key != NULL) {
                memcpy(key, linesp, tokenlen);
                //key[tokenlen] = '\0';
                keys.push_back(key);
                            }
            linesp = next_token;
            tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);     
        }
    } else if (methodsName[methodCode] == "delete" || methodsName[methodCode] == "incr" ||methodsName[methodCode] == "decr" || methodsName[methodCode] == "touch") {
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen]== '\r' )) {
            key = MALLOC(char, tokenlen+1);
            if (key != NULL) {
                memcpy(key, linesp, tokenlen);
                //key[tokenlen] = '\0';
                keys.push_back(key);
            }

            if (methodsName[methodCode] == "touch") {
                linesp = next_token;
                tokenlen = Protocol::get_token_len(linesp,eoh, &next_token);
                if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen] == '\r')) {
                    if (sscanf(linesp, "%u", &val) != 1) {
                        expiration=val;
                    }
                }
            }
        }
    } else if (methodsName[methodCode] == "gat" || methodsName[methodCode] == "gats") {
        tokenlen = Protocol::get_token_len(linesp,eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen] == '\r')) {
            if (sscanf(linesp, "%u", &val) != 1) {
                expiration=val;
            }

            while (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
                linesp = next_token;
                tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
                key = MALLOC(char, tokenlen+1);
                if (key != NULL) {
                    memcpy(key, linesp, tokenlen);
                    //key[tokenlen] = '\0';
                    keys.push_back(key);
                }    
            }
        }
    }
}

_memcached::Response::Response() {
    char statusName[20][50] = { "NONE", "VALUE", "OK", "END", "STORED", "NOT_STORED", "EXISTS",
    "NOT_FOUND", "ERROR", "CLIENT_ERROR", "SERVER_ERROR", "DELETED", "TOUCHED",
    "BUSY", "BADCLASS", "NOTFULL", "UNSAFE", "SAME", "STAT", "VERSION" };

    int cnt = sizeof(statusName)/sizeof(statusName[0]);
    for (int i = 0; i != cnt; ++i) {
        status.insert({i, statusName[i]});
    }
}

/*
 * Extract response message from data.
 * But only the header fields are extracted.
 */
_memcached::Response::Response(const char *data, const char *dataend, long ack) : Response() {
	char *eoh, *eol, *linesp;
	int line_cnt = 0, hdl = 0,tokenlen=0;
    char *next_token, *key;
    unsigned int val;
    unsigned char Flgs;
        
	eoh = Protocol::find_line_end(data, dataend, &eol);
	hdl = eoh -data + 1;
	hdlen = hdl;
	
	/** Added Functionality: Store acknowledgement number
	 *  in response_header
	**/
	acknowledgement = ack;

	/* first line */
	linesp = (char*) data;
	statusCode = parseStatus(linesp, hdl);
	
    if (status[statusCode] == "NONE"){
		return;
	}
    
    if (status[statusCode] == "VALUE")
    {
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        if (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
            key = MALLOC(char, tokenlen+1);
            if (key != NULL) {
                memcpy(key, linesp, tokenlen);
                //key[tokenlen] = '\0';
                keys.push_back(key);
            }
        }
        linesp = next_token;
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        
        if (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
            if (sscanf(linesp, "%hhu", &Flgs) != 1) {
                flags = Flgs;
            }
        }
            
        linesp = next_token;
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' ||linesp[tokenlen] == '\r')) {
            if (sscanf(linesp, "%u", &val) != 1) {
                bodylen = val;
            }
        }
    }	
}