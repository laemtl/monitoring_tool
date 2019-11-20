#include "responseReceived.hpp"

void ResponseReceived::notify(pair_t *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onResponseReceived(pair, flow);
    }
}