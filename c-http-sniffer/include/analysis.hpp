#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include "addr.hpp"
#include "protocol.hpp"
#include <vector>
#include <cstdint>

using namespace std;

class Analysis {
    public:
        bool debug;
        int livemode;
        int socket;
        const char* interface;
        uint32_t interval;
   
        bool serverMode;        
        pthread_mutex_t lock;
        
        // 0: started, 1: running, -1: stopped
        int status;
        uint32_t int_step;
        uint32_t duration;
        
        bool hasClientIp;
        bool hasClientPort;
        Addr client;

        bool hasServerIp;
        bool hasServerPort;
        Addr server;
        
        Addr destination;
        
        vector<Protocol*> protocols;

        // raw packet queue
        Queue* rpq;

        Analysis(int socket, const char* netInt, uint32_t interval, uint32_t duration, bool serverMode, bool debug); 

        bool isServerMode();
        void setClientIp(u_int32_t ip);
        void setClientPort(u_int16_t port);
        void setServerIp(u_int32_t ip);
        void setServerPort(u_int16_t port);
        
        void stop();
        bool isStopped();
};

#endif