#ifndef __METRIC_MANAGER_H__
#define __METRIC_MANAGER_H__

#include <vector> 
#include "protocol.hpp"

using namespace std;

class Analysis;
class EventManager;
class Metric;

class MetricManager {
    public:
        MetricManager(Protocol* protocol, Analysis* analysis);    
        void registerMetrics(vector<bool> status, EventManager* em);
        //void getMetrics();
        vector<Metric*> metrics;
};

#include "analysis.hpp"
#include "eventManager.hpp"
#include "metric.hpp"

#endif