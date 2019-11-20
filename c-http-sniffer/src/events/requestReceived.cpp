#include "requestReceived.hpp"

void RequestReceived::notify(pair_t *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onRequestReceived(pair, flow);
    }
}