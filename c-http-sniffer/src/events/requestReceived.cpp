#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class RequestReceived: public Event {
    public:
        void notify(http_pair_t *pair, Flow *flow);
};

void RequestReceived::notify(http_pair_t *pair, Flow *flow) {
    for (Observer *observer : observers) {
        observer.onRequestReceived(pair, flow);
    }
}