#include "attr.hpp"
#include "conn.hpp"
#include <inttypes.h>

// Get new connection count per interval

class ConnRate: public MetricCumDistr
{
	private:
		hash_table ht;

	public:
		ConnRate();
		onAnalysisEnd();
		void print();
}

ConnRate::ConnRate() {
	hash_init(ht, conn_hash_fn, addr_compare, update_attr, delete_attr);
}

ConnRate::onAnalysisEnd() {

}

u_int32_t conn_hash_fn(Attr* c) {
    return ((((Addr*)c->elem)->port & 0xff) | ((((Addr*)c->elem)->ip & 0xff) << 8)) % ht.size;
}

int addr_compare(Attr* c1, Attr* c2) {
    if((((Addr*)c1->elem)->ip == ((Addr*)c2->elem)->ip) && (((Addr*)c1->elem)->port == ((Addr*)c2->elem)->port)) return 0;
    return 1;
}

/*BOOL is_conn_ht(hash_t* ht) {
	if(ht == &ht) return TRUE;
	return FALSE;
}*/

double getAvg() {
	return (double)data->conn_rate.getSum()/analysis.interval;
}

void ConnRate::print() {
	if(isActive())
		printf("CONN RATE (avg, min, max): %f %f %f \n", result->conn_rate, result->conn_rate_min, result->conn_rate_max);
}

void onNewFlowReceived(Flow *flow) { 
	if(!isActive()) return;
	
	/*u_int32_t saddr = flow->socket.saddr;
	u_int16_t sport = flow->socket.sport;

	Addr* addr = CALLOC(Addr, 1);
	addr->ip = saddr;
	addr->port = sport;
	//add_attr(addr, &ht);
	hash_add(new Attr(addr), &ht);*/

	cnt++;
}


// on new request
// int_cnt++

void Rst::onTimerExpires() {
	if(!isActive()) return;
		int conn_int_cnt = ht.int_cnt;
		addSum(conn_int_cnt);
		setMin(conn_int_cnt);
		setMax(conn_int_cnt);
			
		// reset interval (1 sec) counter
		reset_int_cnt(&ht);
	}
}