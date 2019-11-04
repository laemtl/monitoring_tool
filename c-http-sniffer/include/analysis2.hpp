#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include "eventManager.hpp"
#include "metricManager.hpp"

//class EventManager;
//class MetricManager;

class Analysis {

    public:
        int socket;
        const char* interface;
        uint32_t interval;
   
        EventManager* eventManager;
        MetricManager* metricManager;

        Analysis(int socket, const char* netInt, uint32_t interval); 
        // vector<bool> status
        void activeMetrics(int activeMetrics);
};

#endif