#ifndef __METRIC_MANAGER_H__
#define __METRIC_MANAGER_H__

// or dimplify with functions in analysis
#include "metric.hpp"
#include "rst.hpp"
#include <vector> 
#include <iostream>

// activate metrics
// register particular metrics to events
// must be done before events
// on analysis start


//#include "metricManager.hpp"
//MetricManager* metricManager = new MetricManager();

using namespace std;

class MetricManager 
{
    public:
        MetricManager();
        void registerMetrics(vector<bool> status, EventManager* em);
        void getMetrics();

        vector<Metric2*> metrics;
};

#endif