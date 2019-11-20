#ifndef __REQ_RATE_H__
#define __REQ_RATE_H__

#include "metricAvg.hpp"

class ReqRate: public MetricAvg
{
  public:
    ReqRate(Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(pair_t *pair, Flow* flow);
    void onResponseReceived(pair_t *pair, Flow* flow);
    void onTimerExpired();
    double ReqRate::getAvg();
};

#endif