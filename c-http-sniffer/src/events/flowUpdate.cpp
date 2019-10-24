#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class FlowUpdate: public Event {
    public:
        void notify(Flow *flow);
};

void FlowUpdate::notify(Flow *flow) {
    for (Observer *observer : observers) {
        observer.onFlowUpdate(flow);
    }
}