#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <vector>
#include <cstdint>
#include <pthread.h>

class Queue;
namespace _protocol {
    class Protocol;
}

class Analysis {
    public:
        bool debug;
        int livemode;
        int socket;
        const char* interface;
        uint32_t interfaceId;
        uint32_t interval;
   
        bool serverMode;        
        pthread_mutex_t lock;
        
        // 0: started, 1: running, -1: stopped
        int status;
        uint32_t int_step;
        uint32_t duration;
        
        std::vector<_protocol::Protocol*> protocols;

        // raw packet queue
        Queue* rpq;

        Analysis(int socket, const char* netInt, uint32_t netIntId, uint32_t interval, uint32_t duration); 
        bool isServerMode();
        void stop();
        bool isStopped();
};

#include "addr.hpp"
#include "protocol.hpp"

#endif