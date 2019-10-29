#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class NewFlowReceived: public Event {
    public:
        void notify(flow_t *flow);
};

void NewFlowReceived::notify(flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onNewFlowReceived(flow);
    }
}