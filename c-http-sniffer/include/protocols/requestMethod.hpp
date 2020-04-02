#ifndef __REQUESTMETHOD_H__
#define __REQUESTMETHOD_H__

#include <algorithm>
#include <vector>
#include "protocol.hpp"

class RequestMethod : public _protocol::Request {
    public:
        std::vector<char*> methodsName;
        int methodCode;  

        virtual int parseMethod(const char *data, int linelen) = 0;

        char* getMethodName(int methodsCode) {
            if(methodsCode < 0 || methodsCode > getMethodCount()-1) return NULL; 
            return methodsName[methodsCode];
        }
        
        int getMethodCount() {
            return static_cast<int>(methodsName.size());
        }

        int getMethodCode() {
            return methodCode;
        }
};

#endif