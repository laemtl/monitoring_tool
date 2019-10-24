#include "event.hpp"
#include "observer.hpp"
#include "flow.hpp"

class ResponseReceived: public Event {
    public:
        void notify(http_pair_t *pair, Flow *flow);
};

void ResponseReceived::notify(http_pair_t *pair, Flow *flow) {
    for (Observer *observer : observers) {
        observer.onResponseReceived(pair, flow);
    }
}