#include "reqPath.hpp"
#include <inttypes.h>
#include <libgen.h>
#include <string.h>

#define MAGIC_HASH_NUMBER 5381

class ReqURI: public MetricCumDistr
{
	private:
		hash_table ht;
		int req_tot;

	public:
		ReqPath();
		void print();
}

ReqURI::ReqPath() {
	req_tot = 0;
	hash_init(ht, req_path_hash_fn, req_path_compare, update_attr, delete_attr);
}

// djb2 has excellent distribution and speed
u_int32_t req_path_hash_fn(Attr* a) {
	Data* data = {0};
	get_data(&data);
    
    unsigned char *str = a->elem;

    int c;
    unsigned long hash = MAGIC_HASH_NUMBER;
	while ((c = *str++))
            hash = ((hash << 5) + hash) + c;

	if(hash < 0) hash = -(hash);
    return hash % data->req_path_ht.size;
}

int req_path_compare(Attr* a1, Attr* a2) {
	return strcmp(a1->elem, a2->elem);
}

char* extract_req_path(const char* uri) {
    char *req_path;
	u_int32_t len; 
	const char *query_start;
	//const char *query_end;

	query_start = strchr(uri, '?');
	if(query_start == NULL) {
		query_start = uri + strlen(uri);
	}

	len = query_start - uri;     
	req_path = CALLOC(char, len+1);
	strncpy(req_path, uri, len);

    return req_path;
}

char* extract_req_query(const char* uri) {
    char *query;
	u_int32_t len; 
	const char *query_start;
	//const char *query_end;

	query_start = strchr(uri, '?');
	if(query_start == NULL) {
		query_start = uri + strlen(uri);
	}

	query_start++;
    //query_end = strchr(query_start, '\0');

    len = strlen(query_start);
    query = CALLOC(char, len+1);
    strcpy(query, query_start);

    return query;
}

void req_path_cfl_add(void* req_path, int cnt, Result* r) {
	Data* data = {0};
	get_data(&data);
	if(req_tot <= 0) return;
	
	char* rp = (char*)req_path;

	double freq = (double) cnt / req_tot;
	if(freq > MIN_FREQ) {
		char* p1 = strdup(rp);
		char* m = basename(p1);
		int len1 = strlen(m);   
		char* method = CALLOC(char, len1+1);
		strcpy(method, m);
		free(p1);
		cfl_add(method, freq, &(r->req_method));

		char* p2 = strdup(rp);
		char* d = dirname(p2);
		int len2 = strlen(d);   
		char* dir = CALLOC(char, len2+1);
		strcpy(dir, d);
		free(p2);
		cfl_add(dir, freq, &(r->req_path));	
	}
}

BOOL is_path_ht(hash_t* ht) {
	Data* data = {0};
	get_data(&data);
	
	if(ht == &(data->req_path_ht)) return TRUE;
	return FALSE;
}

void ReqURI::onRequestReceived(http_pair_t *h, Flow *flow) {
	if(!isActive()) return;

	req_tot++;
	const char *uri = h->request_header->uri;
	char* req_path = extract_req_path(uri);
	//add_attr(req_path, &ht);
	hash_add(new Attr(req_path), &ht);
}

void Rst::onTimerExpires() {
	// better in hash

	// reset interval (1 sec) counter
	reset_int_cnt(&ht);
}



// TODO: can have duplicate path (ex: path/meth1 & path/meth2)
if(data->req_path_active || data->req_method_active) {
	cfl_init(&(result->req_path));
	cfl_init(&(result->req_method));
	extract_freq_ht(&(data->req_path_ht), result, req_path_cfl_add);
}

cfl_delete(&(result->req_path));
cfl_delete(&(result->req_method));