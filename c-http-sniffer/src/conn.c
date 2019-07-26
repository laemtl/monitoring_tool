#include "attr.h";
#include "conn.h";
#include "data.h";
#include <inttypes.h>

int conn_hash_fn(Attr* c) {
    Data* data = {0};
	get_data(&data);
    return ((((Addr*)c->elem)->port & 0xff) | ((((Addr*)c->elem)->ip & 0xff) << 8)) % data->conn_ht.size;
}

int ip_compare(Attr* c1, Attr* c2) {
    if((((Addr*)c1->elem)->ip == ((Addr*)c2->elem)->ip)) return 0;
    return 1;
}

void init_conn(Data* data) {
    hash_init(&(data->conn_ht), conn_hash_fn, addr_compare, update_attr);
}

void add_conn(u_int32_t saddr, u_int16_t sport, Data* data) {
    Addr* addr = CALLOC(Addr, 1);
	addr->ip = saddr;
	addr->port = sport;
    add_attr(addr, &(data->client_ht));
}

BOOL is_conn_ht(hash_t* ht) {
	Data* data = {0};
	get_data(&data);
	
	if(ht == &(data->conn_ht)) return TRUE;
	return FALSE;
}