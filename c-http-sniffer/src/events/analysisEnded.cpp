#include "analysisEnded.hpp"

void AnalysisEnded::notify() {
    for (Observer *observer : observers) {
        observer->onAnalysisEnded();
    }
}