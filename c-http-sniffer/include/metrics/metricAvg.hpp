#ifndef __METRIC_AVG_H__
#define __METRIC_AVG_H__

#include "metric.hpp"
//#include "metricValue.hpp"
#include "value.hpp"
#include "observer.hpp"

#include <iostream>
#include <string>
#include <stdio.h>

/*class MAItem {
    double avg;
    double min;
    double max;
};

class MAValue : public MetricValue  
{
	MAItem value;
};*/

using namespace std; 

class MetricAvg: public Metric2, public Observer
{
	protected:
        string name;
		
		Value* subtotal;
        Value* total;
        
        Value* sum;
		Value* subsum;

		Value* avg;
        Value* min;
        Value* max;

		u_int32_t interval;

    public:
		MetricAvg();
		virtual void reset();
		virtual double getAvg();
		void print();
		void onTimerExpired();
		void onIntervalExpired();
		void prepareMsg(double avg, double min, double max);
};

#endif