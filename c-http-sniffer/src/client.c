#include "attr.h";
#include "client.h";
#include "data.h";
#include <inttypes.h>

int client_hash_fn(Attr* c) {
    Data* data = {0};
	get_data(&data);
    return ((((Addr*)c->elem)->port & 0xff) | ((((Addr*)c->elem)->ip & 0xff) << 8)) % data->client_ht.size;
}

int addr_compare(Attr* c1, Attr* c2) {
    if((((Addr*)c1->elem)->ip == ((Addr*)c2->elem)->ip) && (((Addr*)c1->elem)->port == ((Addr*)c2->elem)->port)) return 0;
    return 1;
}

void init_client(Data* data) {
    hash_init(&(data->client_ht), client_hash_fn, addr_compare, update_attr);
}

void add_client(u_int32_t saddr, u_int16_t sport, Data* data) {
    Addr* addr = CALLOC(Addr, 1);
	addr->ip = saddr;
	addr->port = sport;
    add_attr(addr, &(data->client_ht));
}

BOOL is_client_ht(hash_t* ht) {
	Data* data = {0};
	get_data(&data);
	
	if(ht == &(data->client_ht)) return TRUE;
	return FALSE;
}