#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class FlowUpdate: public Event {
    public:
        void notify(flow_t *flow);
};

void FlowUpdate::notify(flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onFlowUpdate(flow);
    }
}