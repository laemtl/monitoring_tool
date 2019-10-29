#include "client.hpp"
#include "cf_list.hpp"
#include <inttypes.h>

class Client: public MetricCumDistr
{
	private:
		int req_tot;
		hash_table ht;

	public:
		Client();
		void print();
}

Client::Client() {
	hash_init(ht, hash_fn, compare, update_attr, delete_attr);
}

u_int32_t hash_fn(Attr* a) {
    if(a == NULL) error("Element cannot be NULL");
    return *(u_int32_t*)(a->elem) % ht.size;
}

int compare(Attr* c1, Attr* c2) {
    if(*(u_int32_t*)c1->elem == *(u_int32_t*)c2->elem) return 0;
    return 1;
}

void client_cfl_add(void* addr, int cnt, Result* r) {
	u_int32_t add = *(u_int32_t*)addr;

	if(req_tot <= 0) return;
	
	double freq = (double) cnt / req_tot;
    if(freq > MIN_FREQ) {
	    cfl_add(ip_ntos(add), freq, &(r->client));
	}
}

BOOL is_client_ht(hash_t* ht) {
	if(ht == &ht) return TRUE;
	return FALSE;
}

void ReqURI::onRequestReceived(http_pair_t *h, flow_t *flow) {
	if(!isActive()) return;

	req_tot++;

	request_t *req = h->request_header;
	u_int32_t* addr = CALLOC(u_int32_t, 1);
	*addr = flow->socket.saddr;
	//add_attr(addr, &ht);
	hash_add(new Attr(addr), &ht);
}

void Rst::onTimerExpires() {
	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}


if(client_active) {
	cfl_init(&(result->client));
	extract_freq_ht(&client_ht, result, client_cfl_add);
}

cfl_delete(&(result->client));