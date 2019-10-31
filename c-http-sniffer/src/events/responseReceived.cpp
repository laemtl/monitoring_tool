#include "responseReceived.hpp"

void ResponseReceived::notify(http_pair_t *pair, flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onResponseReceived(pair, flow);
    }
}