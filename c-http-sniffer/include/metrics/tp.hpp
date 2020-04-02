#ifndef __TP_H__
#define __TP_H__

#include "metricAvg.hpp"

class Tp: public MetricAvg
{
  private:
    u_int32_t timerRef;
    u_int32_t intervalRef;
    
    public:
      Tp(_protocol::Protocol* protocol, Analysis* analysis);
      void subscribe(EventManager* em);
      void onNewFlowReceived(Flow* flow);
      void onFlowUpdate(Flow* flow);
      void onRequestReceived(_protocol::Pair *pair, Flow* flow);
      void onResponseReceived(_protocol::Pair *pair, Flow* flow);
      void onTimerExpired();
      
      void reset();
      double getAvg();
};

#include "flow.hpp"

#endif