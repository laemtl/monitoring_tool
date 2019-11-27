#include "responseReceived.hpp"

void ResponseReceived::notify(Pair *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onResponseReceived(pair, flow);
    }
}