#ifndef __METRIC_VALUE_H_
#define __METRIC_VALUE_H_

#include <string>
#include <pthread.h>

class MetricValue
{
    public:
        char* interface;
        int client_sock;
        std::string name;
        int type;
};

#endif
