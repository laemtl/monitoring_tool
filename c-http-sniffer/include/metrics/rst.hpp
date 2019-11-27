#ifndef __RST_H__
#define __RST_H__

#include "metricAvg.hpp"
#include <inttypes.h>


class Rst: public MetricAvg
{
  public:
    Rst(Protocol* protocol, Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(Pair *pair, Flow* flow);
    void onResponseReceived(Pair *pair, Flow* flow);
    void onTimerExpired();
};

#endif