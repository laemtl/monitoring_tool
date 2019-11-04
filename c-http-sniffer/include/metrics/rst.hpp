#ifndef __RST_H__
#define __RST_H__

#include "metricAvg.hpp"
#include <inttypes.h>


class Rst: public MetricAvg
{
  public:
    Rst(Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(flow_t *flow);
    void onFlowUpdate(flow_t *flow);
    void onRequestReceived(http_pair_t *pair, flow_t *flow);
    void onResponseReceived(http_pair_t *pair, flow_t *flow);
    void onTimerExpired();
};

#endif