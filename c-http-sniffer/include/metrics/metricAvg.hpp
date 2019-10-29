#ifndef __METRIC_AVG_H__
#define __METRIC_AVG_H__

#include "metric.hpp"
#include "metricValue.hpp"

/*class MAItem {
    double avg;
    double min;
    double max;
};

class MAValue : public MetricValue  
{
	MAItem value;
};*/

class MetricAvg: public Metric2
{
	private:
        int typeId;

    public:
		MetricAvg();
		double getAvg();
		void print();
		void onIntervalExpired();
		void prepareMsg(double avg, double min, double max);
};

#endif