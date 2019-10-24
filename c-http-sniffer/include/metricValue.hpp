#ifndef __METRIC_VALUE_H_
#define __METRIC_VALUE_H_

#include <string>
#include <pthread.h>

using namespace std;

class MetricValue
{
    public:
        char* interface;
        int client_sock;
        string name;
        int type;
};

#endif