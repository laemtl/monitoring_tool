#ifndef __METRIC_H__
#define __METRIC_H__

#include <string>

class Analysis;
class EventManager;
namespace _protocol {
    class Protocol;
}

class Metric {
    public:  
        std::string name;
        std::string desc;
		_protocol::Protocol* protocol;
        Analysis* analysis;
        Metric(_protocol::Protocol* protocol, Analysis* analysis, std::string name, std::string desc) : analysis(analysis), protocol(protocol), name(name), desc(desc) {};
        virtual void subscribe(EventManager* em) = 0;
};

#include "analysis.pb-c.h"

#endif