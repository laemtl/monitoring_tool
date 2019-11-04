#ifndef __TP_H__
#define __TP_H__

#include "metricAvg.hpp"

class Tp: public MetricAvg
{
  private:
    u_int32_t timerRef;
    u_int32_t intervalRef;
    
    public:
      Tp(Analysis* analysis);
      void subscribe(EventManager* em);
      void onNewFlowReceived(flow_t *flow);
      void onFlowUpdate(flow_t *flow);
      void onRequestReceived(http_pair_t *pair, flow_t *flow);
      void onResponseReceived(http_pair_t *pair, flow_t *flow);
      void onTimerExpired();
      
      void reset();
      double getAvg();
};

#endif