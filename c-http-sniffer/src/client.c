#include "client.h";
#include <inttypes.h>

u_int32_t client_hash_fn(Attr* a) {
    if(a == NULL) return;

    Data* data = {0};
	get_data(&data);

    return *(u_int32_t*)(a->elem) % data->client_ht.size;
}

int ip_compare(Attr* c1, Attr* c2) {
    if((((Addr*)c1->elem)->ip == ((Addr*)c2->elem)->ip)) return 0;
    return 1;
}

void client_cfl_add(Addr* addr, int cnt, Result* r) {
	Data* data = {0};
	get_data(&data);
	if(data->flow_tot <= 0) return;
	
	double freq = (double) cnt / data->flow_tot;
   
    if(freq > MIN_FREQ) {
		char* c = addr_to_str(addr->ip);
	    cfl_add(c, freq, &(r->client));
	}
}

void add_client(u_int32_t saddr, Data* data) {
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