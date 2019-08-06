#include "data.h"
#include "raw_packet.h"

void raw_packet_free(raw_pkt* p) {
    free(p->raw);
    free(p);
}

void raw_pkt_queue_init() {
    Data* data = {0};
	get_data(&data);

    data->raw_pkt_queue.free = raw_packet_free;
    queue_init(&(data->raw_pkt_queue));
}