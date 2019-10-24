#ifndef __TCP_H__
#define __TCP_H__

#include "order.hpp"
#include "util.hpp"

int tcp_order(Order *ord, Seq *new_seq, bool src);
int tcp_order_check(Order *order);	// for debugging

#endif
