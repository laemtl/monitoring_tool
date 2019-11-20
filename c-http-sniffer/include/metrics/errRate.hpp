#ifndef __ERR_RATE_H__
#define __ERR_RATE_H__

#include "metricAvg.hpp"

class ErrRate: public MetricAvg
{
  public:
    ErrRate(Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(pair_t *pair, Flow* flow);
    void onResponseReceived(pair_t *pair, Flow* flow);
    void onTimerExpired();

    double getRate();
};

#endif