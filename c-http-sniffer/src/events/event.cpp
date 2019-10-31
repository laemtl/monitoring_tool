#include "event.hpp"

void Event::add(Observer *obs) {
    observers.push_back(obs);
}

void Event::remove(Observer *obs) {
    // find the observer
    auto iterator = find(observers.begin(), observers.end(), obs);

    if (iterator != observers.end()) { // observer found
        observers.erase(iterator); // remove the observer
    }
}