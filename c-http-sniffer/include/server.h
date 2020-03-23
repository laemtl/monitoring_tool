#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>

#include "packet.h"
#include "analysis.pb-c.h"
#include "analysis.hpp"
#include "timer.h"
#include "http.hpp"
#include "memcached.hpp"

class Config {
    public:
        int socket; 
        bool debug;
        Config(int socket, bool debug) : socket(socket), debug(debug) {};
};

void start_log(char* ipaddress);
void start_server(void* debug);
void start_analysis(char* ipaddress, Analysis* analysis);
size_t decode_varint(int sock);
int encode_varint(uint8_t *const buffer, uint64_t value);
void connection_handler(Config* config);
void stop_server();
void onExit(int signum);

/*typedef struct analysis_conn_ {
    uint32_t socket;
} Client;*/

#endif
