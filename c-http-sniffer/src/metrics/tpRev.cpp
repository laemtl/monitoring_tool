
#include "tpRev.hpp"

TpRev::TpRev() {
	name = "TP REV NEW";
}

void TpRev::onFlowUpdate(flow_t *flow) {
	u_int32_t payload = flow->payload_dst;
	total->set(payload);
}