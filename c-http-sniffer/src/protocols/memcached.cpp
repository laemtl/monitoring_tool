#include "memcached.hpp"

MemCached::MemCached(Analysis* analysis) : Protocol(analysis) {
    //reqType = new int[statusCode.size()]{};
    ports.insert(ports.end(), {11211});

    methodName = {
        "get",
        "set",
        "add",
        "replace",
        "append",
        "prepend",
        "delete",
        "flush_all",
        "quit",
        "incr",
        "decr",
        "touch",
        "gat",
        "gats",
        "slabs",
        "lru",
        "watch",
        "stats",
        "version",
        "misbehave",
        "NONE"
    };
}

/*
 * From xplico.
 * Get Cache request method by parsing header line.
 */
Method _memcached::parseMethod(const char *data, int linelen)
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

    /* Check the methods that have same length */
    switch (index) {
        case 3:
            if (strncmp(data, "get", index) == 0) {
                //totalGet++;
                return get;
            } else if (strncmp(data, "set", index) == 0) {
                //totalSet++;
                return set;
            } else if (strncmp(data, "add", index) == 0) {
                return add;
            } else if (strncmp(data, "gat", index) == 0) {
                return gat;
            } else if (strncmp(data, "lru", index) == 0) {
                return lru;
            }

            break;

            case 4:
                if (strncmp(data, "quit", index) == 0) {
                    return quit;
                } else if (strncmp(data, "incr", index) == 0) { 
                    return incr;
                } else if (strncmp(data, "decr", index) == 0) {
                    return decr;
                } else if (strncmp(data, "gats", index) == 0) {
                    return gats;
                }
                
                break;

            case 5:
                if (strncmp(data, "touch", index) == 0) {
                    return touch;
                } else if (strncmp(data, "slabs", index) == 0) {
                    return slabs;
                } else if (strncmp(data, "watch", index) == 0) {
                    return watch;
                } else if (strncmp(data, "stats", index) == 0) {  
                    return stats;
                }
                
                break;

            case 6:
                if (strncmp(data, "append", index) == 0) {
                    return append;
                } else if (strncmp(data, "delete", index) == 0) {
                    return deletem;
                }
                
                break;

            case 7:
                if (strncmp(data, "replace", index) == 0) {
                    return replace;
                } else if (strncmp(data, "prepend", index) == 0) {   
                    return prepend;
                } else if (strncmp(data, "version", index) == 0) {  
                    return version;
                }
                
                break;

            case 9:
                if (strncmp(data, "flush_all", index) == 0) {
                    return flush_all;
                } else if (strncmp(data, "misbehave", index) == 0) {
                    return misbehave;
                }
                
                break;

            default:
                break;
        }
        return CACHE_MT_NONE;
}


/*
 * To identify if the packet is carrying Cache request message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* MemCached::isRequest(const char *ptr, const int datalen) {
	Method method = CACHE_MT_NONE;
	char *eol,*linesp,*head_end = NULL;
        int lnl;
        linesp = (char*) ptr;
	head_end = Protocol::find_line_end(linesp, datalen, &eol);
	lnl = head_end - linesp + 1;
	method = parseMethod(linesp, lnl);
	if (method == CACHE_MT_NONE){
		return NULL;
	}
	else{
		return head_end;
	}
}

Status _memcached::parseStatus(const char *line, int len) {
    const char *next_token;
    const char *lineend;
    const char *st;
    Status status;
    int tokenlen;
    
    lineend = line + len;
    status = CACHE_ST_NONE;

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
    st = MALLOC(char, tokenlen+1);
    if (st != NULL) {
        memcpy(st, line, tokenlen);
        //st[tokenlen] = '\0';
    }
    
    /* search enum */
    map<const char*, Status>::iterator it;
    it = statusCode.find(st);
    if (it != statusCode.end()) status = it->second;
    free(st);

    return status;
}

/*
 * To identify if the packet is carrying cache response message.
 * If it's true, the head end char pointer will be returned, else NULL.
 */
char* MemCached::isResponse(const char *ptr, const int datalen) {
	char *eol, *linesp, *head_end = NULL;
    int lnl;
    Status status = CACHE_ST_NONE;
    linesp = (char*) ptr;
	head_end = Protocol::find_line_end(linesp, datalen, &eol);
	lnl = head_end - linesp + 1;
	status = parseStatus(linesp, lnl);
    
    if (status = CACHE_ST_NONE){
		return NULL;
	} else {
		return head_end;
	}
}

bool MemCached::isPacketOf(u_int16_t sport, u_int16_t dport) {
    if(find(ports.begin(), ports.end(), sport) != ports.end()
    || find(ports.begin(), ports.end(), dport) != ports.end()) {
		return true;
	}
    return false;
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
        vector<const char*>(keys).swap(keys);
    if(time != NULL)
        free(time);
}

_memcached::Response::~Response() {
     if (!keys.empty())
        vector<const char*>(keys).swap(keys);
}


_protocol::Request* MemCached::getRequest(const char *data, const char *dataend, char* time, u_int32_t seq, u_int32_t nxt_seq) {
    return new Request(data, dataend, time, seq, nxt_seq);
}

_protocol::Response* MemCached::getResponse(const char *data, const char *dataend, long ack) {
    return new Response(data, dataend, ack);
}

char* MemCached::getMethodName(int m) {
    return methodName[m];
}

int MemCached::getMethodCount() {
    return static_cast<int>(methodName.size());
}

int MemCached::getStatusCount() {
    return static_cast<int>(statusCode.size());
}

/*
 * Extract request message from data.
 * But only the header fields are extracted.
 */

_memcached::Request::Request(const char *data, const char *dataend, char *time, u_int32_t seq, u_int32_t nxt_seq) {
	char *eoh, *eol, *linesp, *lineep;
	int tokenlen = 0, hdl = 0;
        const char *next_token, *key;
        unsigned int val;

    seq = seq;
    nxt_seq = nxt_seq;

	eoh = Protocol::find_line_end(data, dataend, &eol);
	hdl = eoh - data + 1;
	hdlen = hdl;
	time = strdup(time);
	/* Parse first line of cache request */
	linesp = (char*) data;
	method = parseMethod(linesp, hdl);
	if ( method == CACHE_MT_NONE){
        return;
	}
    
    if(method == get /*|| method == gets*/) {
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        while (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
            key = MALLOC(char, tokenlen+1);
            if (key != NULL) {
                memcpy(key, linesp, tokenlen);
                //key[tokenlen] = '\0';
                keys.push_back(key);
                            }
            linesp=next_token;
            tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);     
        }
    } else if (method==deletem || method==incr ||method==decr || method==touch) {
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen]== '\r' )) {
            key = MALLOC(char, tokenlen+1);
            if (key != NULL) {
                memcpy(key, linesp, tokenlen);
                //key[tokenlen] = '\0';
                keys.push_back(key);
            }

            if (method == touch) {
                linesp=next_token;
                tokenlen = Protocol::get_token_len(linesp,eoh, &next_token);
                if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen] == '\r')) {
                    if (sscanf(linesp, "%u", &val) != 1) {
                        expiration=val;
                    }
                }
            }
        }
    } else if (method == gat || method == gats) {
        tokenlen = Protocol::get_token_len(linesp,eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' || linesp[tokenlen] == '\r')) {
            if (sscanf(linesp, "%u", &val) != 1) {
                expiration=val;
            }

            while (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
                linesp=next_token;
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
    statusCode["VALUE"]=VALUE;
    statusCode["OK"]=OK;
    statusCode["END"]=END;
    statusCode["STORED"]=STORED;
    statusCode["NOT_STORED"]=NOT_STORED;
    statusCode["EXISTS"]=EXISTS;
    statusCode["NOT_FOUND"]=NOT_FOUND;
    statusCode["ERROR"]=ERROR;
    statusCode["CLIENT_ERROR"]=CLIENT_ERROR;
    statusCode["SERVER_ERROR"]=SERVER_ERROR;
    statusCode["DELETED"]=DELETED;
    statusCode["TOUCHED"]=TOUCHED;
    statusCode["BUSY"]=BUSY;
    statusCode["BADCLASS"]=BADCLASS;
    statusCode["NOTFULL"]=NOTFULL;
    statusCode["UNSAFE"]=UNSAFE;
    statusCode["SAME"]=SAME;
    statusCode["STAT"]=STAT;
    statusCode["VERSION"]=VERSION;
}

/*
 * Extract response message from data.
 * But only the header fields are extracted.
 */
_memcached::Response::Response(const char *data, const char *dataend, long ack) : Response() {
	char *eoh, *eol, *linesp;
	int line_cnt = 0, hdl = 0,tokenlen=0;
    const char *next_token,*key;
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
	status = parseStatus(linesp, hdl);
	
    if (status == CACHE_ST_NONE){
		return;
	}
    
    if (status==VALUE)
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
        linesp=next_token;
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        
        if (tokenlen != 0 && linesp[tokenlen] == ' ' ) {
            if (sscanf(linesp, "%hhu", &Flgs) != 1) {
                flags=Flgs;
            }
        }
            
        linesp=next_token;
        tokenlen = Protocol::get_token_len(linesp, eoh, &next_token);
        if (tokenlen != 0 && (linesp[tokenlen] == ' ' ||linesp[tokenlen] == '\r')) {
            if (sscanf(linesp, "%u", &val) != 1) {
                bodylen=val;
            }
        }
    }	
}