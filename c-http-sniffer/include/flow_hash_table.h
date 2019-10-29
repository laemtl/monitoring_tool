#ifndef FLOW_HTABLE_H
#define FLOW_HTABLE_H

#include "analysis.hpp"

int flow_hash_init(void* d);
flow_t* flow_hash_new(flow_s s);
flow_t* flow_hash_delete(flow_t *f);
flow_t* flow_hash_find(flow_s s);
int flow_hash_add_packet(packet_t *packet);
int flow_hash_clear(void* d);
void flow_hash_reset(Data* data);
int flow_hash_size(void);
int flow_hash_fcnt(void);
int flow_hash_scnt(void);
int flow_scrubber(const int timeout);
void flow_hash_print(void);

#endif
