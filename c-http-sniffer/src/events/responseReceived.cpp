#include "responseReceived.hpp"

void ResponseReceived::notify(_protocol::Pair *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onResponseReceived(pair, flow);
    }
}