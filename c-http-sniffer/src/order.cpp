//#include <stdlib.h>
//#include <string.h>
//#include "packet.hpp"
//#include "util.hpp"
#include "order.hpp"

Order::Order() 
	: src(NULL), dst(NULL), last_src(NULL), last_dst(NULL), num(0), rst(false) 
{}

Order::~Order(){
	Seq *s = NULL;

	while(src != NULL){
		s = src;
		src = src->next;
		delete s;
	}
	while(dst != NULL){
		s = dst;
		dst = dst->next;
		delete s;
	}
}