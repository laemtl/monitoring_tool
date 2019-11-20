#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include "eventManager.hpp"
#include "metricManager.hpp"
#include "packetQueue.hpp"
#include "flowHashTable.hpp"
#include "flowQueue.hpp"
#include "addr.hpp"
#include "queue.hpp"
#include "observer.hpp"

class Analysis : Observer {

    public:
        bool debug;
        int livemode;
        int socket;
        const char* interface;
        uint32_t interval;
   
        EventManager* eventManager;
        MetricManager* metricManager;

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
        int req_tot;
        int rsp_tot;
        int flow_tot;
        
        //HttpAnalysis* http;
        Queue* rpq;
        Queue* fq;
        Queue* pq;
        FlowHashTable* fht;

        Analysis(int socket, const char* netInt, uint32_t interval, uint32_t duration, bool serverMode, bool debug); 

        void activeMetrics(int activeMetrics);
        bool isServerMode();
        void setClientIp(u_int32_t ip);
        void setClientPort(u_int16_t port);
        void setServerIp(u_int32_t ip);
        void setServerPort(u_int16_t port);
        void flowHashProcess();
        void extractData(Flow *flow);
        void stop();
        bool isStopped();

        void onNewFlowReceived(Flow *flow);
        void onFlowUpdate(Flow *flow);
        void onRequestReceived(pair_t *pair, Flow *flow);
        void onResponseReceived(pair_t *pair, Flow *flow);
        void onTimerExpired();
        void onIntervalExpired();
        void onAnalysisEnded();
};

#endif