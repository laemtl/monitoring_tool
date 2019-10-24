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
#include <iostream>
#include "metricValue.hpp"
#include <inttypes.h>
#include <unistd.h>

#include "packet.hpp"
#include "analysis.pb.h"
#include "timer.hpp"
#include "util.hpp"

void start_log(char* ipaddress);
void start_server(bool debug);
void start_analysis(char* ipaddress);
int send_data(MetricValue* mValue);
  
/*typedef struct analysis_conn_ {
    uint32_t socket;
} Client;*/

#endif
