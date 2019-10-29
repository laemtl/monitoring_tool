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

void start_log(char* ipaddress);
void start_server();
void start_analysis(char* ipaddress, Data* data);
int send_data(Result* result);
  
/*typedef struct analysis_conn_ {
    uint32_t socket;
} Client;*/

#endif
