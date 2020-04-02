#ifndef __RST_H__
#define __RST_H__

#include <inttypes.h>
#include "metricAvg.hpp"

namespace _protocol {
	class Protocol;
}

class Rst: public MetricAvg
{
  public:
    Rst(_protocol::Protocol* protocol, Analysis* analysis);
    void subscribe(EventManager* em);
    void onNewFlowReceived(Flow* flow);
    void onFlowUpdate(Flow* flow);
    void onRequestReceived(_protocol::Pair *pair, Flow* flow);
    void onResponseReceived(_protocol::Pair *pair, Flow* flow);
    void onTimerExpired();
};

#endif