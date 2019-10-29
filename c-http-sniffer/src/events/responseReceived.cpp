#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class ResponseReceived: public Event {
    public:
        ResponseReceived();
        void notify(http_pair_t *pair, flow_t *flow);
};

ResponseReceived::ResponseReceived() {
    //EventManager.newFlowReceived = this;
}

void ResponseReceived::notify(http_pair_t *pair, flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onResponseReceived(pair, flow);
    }
}