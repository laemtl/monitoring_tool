#ifndef __METRIC_MANAGER_H__
#define __METRIC_MANAGER_H__

#include <vector> 
#include <iostream>
//#include "analysis2.hpp"
#include "eventManager.hpp"
#include "metric.hpp"

using namespace std;

class Analysis;
class MetricManager 
{
    public:
        MetricManager(Analysis* analysis);    
        void registerMetrics(vector<bool> status, EventManager* em);
        void getMetrics();
        vector<Metric2*> metrics;
};

#endif