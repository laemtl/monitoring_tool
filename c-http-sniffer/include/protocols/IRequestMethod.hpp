#ifndef __RESPONSESTATUS_H__
#define __RESPONSESTATUS_H__

#include <algorithm>
#include <vector>

class RequestMethod {
    public:
        std::vector<int> status;
        int statusCode;
        virtual bool hasErrorStatus() = 0;

        int getStatusCount() {
            return static_cast<int>(status.size());
        }

        int getStatusCode(int i) {
            std::vector<int>::iterator itr = std::find(status.begin(), status.end(), i);
 
            if (itr != status.cend()) {
                std::distance(status.begin(), itr);
            } else {
                return -1;
            }
        }
};

#endif