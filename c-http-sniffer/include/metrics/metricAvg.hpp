#ifndef __METRIC_AVG_H__
#define __METRIC_AVG_H__

#include <stdio.h>
#include "metric.hpp"
#include "observer.hpp"

class Value;

namespace _protocol {
	class Protocol;
}

class MetricAvg : public Metric, public Observer
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
		MetricAvg(_protocol::Protocol* protocol, Analysis* analysis, std::string name, std::string desc);
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

#include "value.hpp"
#include "analysis.hpp"
#include "server.h"

#endif