#ifndef __RST_H__
#define __RST_H__

#include "metricAvg.hpp"
#include <inttypes.h>


class Rst: public MetricAvg
{
  public:
    Rst(Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(pair_t *pair, Flow* flow);
    void onResponseReceived(pair_t *pair, Flow* flow);
    void onTimerExpired();
};

#endif