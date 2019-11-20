#ifndef __METRIC_AVG_H__
#define __METRIC_AVG_H__

#include "metric.hpp"
//#include "metricValue.hpp"
#include "value.hpp"
#include "observer.hpp"
#include <stdio.h>
#include "analysis.hpp"
#include "server.h"

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

//class Analysis;
class MetricAvg : public Metric2, public Observer
{
	protected:
		Value* subtotal;
        Value* total;
        
        Value* sum;
		Value* subsum;

		Value* avg;
        Value* min;
        Value* max;

    public:
		MetricAvg(Analysis* analysis, string name, string desc);
		void subscribe(EventManager* em);
		virtual void reset();
		virtual double getAvg();
		void print();
		void onTimerExpired();
		void onIntervalExpired();
		void onAnalysisEnded();
		void prepareMsg(double avg, double min, double max);
		void sendMsg(); 
};

#endif