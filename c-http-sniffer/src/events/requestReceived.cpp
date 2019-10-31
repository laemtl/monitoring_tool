#include "requestReceived.hpp"

void RequestReceived::notify(http_pair_t *pair, flow_t *flow) {
    for (Observer *observer : observers) {
        observer->onRequestReceived(pair, flow);
    }
}