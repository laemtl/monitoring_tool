void ReqType::onRequestReceived(request_t *req) {
    http_mthd type = req->method;
    
    // Atomic increment
    if(!isActive()) return;
    
    int* cnt = &(req_type[type]);  
    __atomic_fetch_add(cnt, 1, __ATOMIC_SEQ_CST);
}

void ReqPath::print() {
	if(isActive()) {
		printf("Method cumulated freq list \n");
		print_cfl(&(result->req_method));
		printf("\n");
	}
}

void ReqType::onRequestReceived(http_pair_t *h, Flow *flow) {
	if(!isActive()) return;

	request_t *req = h->request_header;					
	const char *uri = req->uri;
	char* req_path = extract_req_path(uri);
	hash_add(new Attr(req_path), &ht);
	//add_attr(req_path, &ht);
}