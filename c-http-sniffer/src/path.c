#include "path.h";
#include "data.h";
#include <inttypes.h>

#define MAGIC_HASH_NUMBER 5381

// djb2 has excellent distribution and speed
unsigned long path_hash_fn(Attr* a) {
	Data* data = {0};
	get_data(&data);
    
    unsigned char *str = a->elem;

    int c;
    unsigned long hash = MAGIC_HASH_NUMBER;
	while ((c = *str++))
            hash = ((hash << 5) + hash) + c;

	if(hash < 0) hash = -(hash);
    return hash % data->path_ht.size;
}

int path_compare(Attr* a1, Attr* a2) {
    return strcmp(a1->elem, a2->elem);
}

void init_path(Data* data) {
    hash_init(&(data->path_ht), path_hash_fn, path_compare, update_attr);
	tl_init(&(data->path_ht.tl));
}

char* extract_path(const char* uri) {
    char *path;
	//char *query;
	u_int32_t len; 
	const char *query_start;
	const char *query_end;

	query_start = strchr(uri, '?');
	if(query_start == NULL) {
		query_start = uri + strlen(uri);
	}

	len = query_start - uri;     
	path = CALLOC(char, len+1);
	strncpy(path, uri, len);
	
    /*query_start++;
    query_end = strchr(query_start, '\0');

    len = query_end - query_start;
    query = CALLOC(len+1, char);
    strncpy(query, query_start, len);*/

    return path;
}

void add_path(const char *uri, Data* data) {
	char* path = extract_path(uri);
	add_attr(path, &(data->path_ht));
}

BOOL is_path_ht(hash_t* ht) {
	Data* data = {0};
	get_data(&data);
	
	if(ht == &(data->path_ht)) return TRUE;
	return FALSE;
}