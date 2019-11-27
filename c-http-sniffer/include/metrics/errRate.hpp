#ifndef __ERR_RATE_H__
#define __ERR_RATE_H__

#include "metricAvg.hpp"

class ErrRate: public MetricAvg
{
  public:
    ErrRate(Protocol* protocol, Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(Pair *pair, Flow* flow);
    void onResponseReceived(Pair *pair, Flow* flow);
    void onTimerExpired();

    double getRate();
};

#endif