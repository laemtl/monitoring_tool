#include "event.hpp"

Event::Event() {
}

void Event::add(Observer *obs) {
    observers.push_back(obs);
}

void Event::remove(Observer *obs) {
    // find the observer
    auto iterator = std::find(observers.begin(), observers.end(), obs);

    if (iterator != observers.end()) { // observer found
        observers.erase(iterator); // remove the observer
    }
}