#ifndef __RESPONSESTATUS_H__
#define __RESPONSESTATUS_H__

#include <algorithm>
#include <vector>
#include <map>

class ResponseStatus {
    public:
        std::map<int, char*> status;
        int statusCode;
        virtual bool hasErrorStatus() = 0;
        virtual int parseStatus(const char *line, int len) = 0;

        int getStatusCount() {
            return static_cast<int>(status.size());
        }

        char* getStatusName(int statusCode) {
            if ( status.find(statusCode) == status.end() ) {
                // not found
                return NULL;
            } else {
                // found
                return status[statusCode];
            }
        }   
};

#endif