#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class NewFlowReceived: public Event {
    public:
        void notify(Flow *flow);
};

void NewFlowReceived::notify(Flow *flow) {
    for (Observer *observer : observers) {
        observer.onNewFlowReceived(flow);
    }
}