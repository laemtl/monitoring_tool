#ifndef __REQ_RATE_H__
#define __REQ_RATE_H__

#include "metricAvg.hpp"

class ReqRate: public MetricAvg
{
  public:
    ReqRate(_protocol::Protocol* protocol, Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(_protocol::Pair *pair, Flow* flow);
    void onResponseReceived(_protocol::Pair *pair, Flow* flow);
    void onTimerExpired();
    double ReqRate::getAvg();
};

#endif