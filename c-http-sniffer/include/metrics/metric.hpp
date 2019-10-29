#ifndef __METRIC_H__
#define __METRIC_H__
 
#include "value.hpp"
#include "observer.hpp"
#include "eventManager.hpp"
//#include "analysis.hpp"

class Metric2: public Observer
{
    protected:  
        string name;
        bool active;
        //Analysis *analysis;
        Value* subtotal;
        Value* total;
        
        Value* sum;
        Value* min;
        Value* max;

    public:   
        Metric2();

        virtual void subscribe(EventManager* em);

        bool isActive();
        void setActive(bool a);
        void reset();
        /*double getSubtotal();
        double getTotal();
        void setTotal(double n);
        double getSum();
        double getMax();
        double getMin();
        void addSum(double n);
        void addTotal(double n);
        void addSubtotal(double n);*/
        void resetSubtotal();
        void setMin(double n);
        void setMax(double n);
};

#endif