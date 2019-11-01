#ifndef __METRIC_MANAGER_H__
#define __METRIC_MANAGER_H__

#include "rst.hpp"
#include "reqRate.hpp"
#include "errRate.hpp"
#include "tp.hpp"
#include "tpRev.hpp"
#include "connRate.hpp"
#include "client.hpp"
#include <vector> 
#include <iostream>

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