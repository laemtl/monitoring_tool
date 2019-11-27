#include "requestReceived.hpp"

void RequestReceived::notify(Pair *pair, Flow* flow) {
    for (Observer *observer : observers) {
        observer->onRequestReceived(pair, flow);
    }
}