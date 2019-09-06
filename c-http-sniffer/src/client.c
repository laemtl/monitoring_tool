#include "client.h";
#include <inttypes.h>

u_int32_t client_hash_fn(Attr* a) {
    if(a == NULL) return;

    Data* data = {0};
	get_data(&data);

    return *(u_int32_t*)(a->elem) % data->client_ht.size;
}

int ip_compare(Attr* c1, Attr* c2) {
    if(*(u_int32_t*)c1->elem == *(u_int32_t*)c2->elem) return 0;
    return 1;
}

void client_cfl_add(u_int32_t* addr, int cnt, Result* r) {
	Data* data = {0};
	get_data(&data);

	if(data->flow_tot <= 0) return;
	
	double freq = (double) cnt / data->req_tot;
	//double freq = (double) cnt / data->flow_tot;
    if(freq > MIN_FREQ) {
	    cfl_add(ip_ntos(*addr), freq, &(r->client));
	}
}

void add_client(u_int32_t saddr, Data* data) {
	if(!data->client_active) return;

    u_int32_t* addr = CALLOC(u_int32_t, 1);
    *addr = saddr;
    add_attr(addr, &(data->client_ht));
}

BOOL is_client_ht(hash_t* ht) {
	Data* data = {0};
	get_data(&data);
	
	if(ht == &(data->client_ht)) return TRUE;
	return FALSE;
}