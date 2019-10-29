#include "event.hpp"
#include "observer.hpp"
#include "flow.h"

class RequestReceived: public Event {
    public:
        void notify(http_pair_t *pair, flow_t *flow);
};

void RequestReceived::notify(http_pair_t *pair, flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onRequestReceived(pair, flow);
    }
}