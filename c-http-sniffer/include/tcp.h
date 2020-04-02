/*
 * tcp.h
 *
 *  Created on: Jun 15, 2011
 *      Author: chenxm
 */

#ifndef __TCP_H__
#define __TCP_H__

int tcp_order(order_t *ord, seq_t *new_seq, bool src);
int tcp_order_check(order_t *order);	// for debugging

#include "order.h"
#include "util.h"

#endif /* __TCP_H__ */
