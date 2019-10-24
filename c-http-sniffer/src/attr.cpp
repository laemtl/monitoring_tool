#include "attr.hpp"

Attr::Attr(void* e) {
    elem = e;
    pthread_mutex_init(&mutex, NULL);
}

Attr::~Attr(){
    free(elem);
}

void Attr::update() {
    pthread_mutex_lock(&mutex);
	cnt++;
    pthread_mutex_unlock(&mutex);   
}

int Attr::compare(Attr* a) {
    if(a == NULL) {
        error("Uninitilized value \n");
    }

    if(cnt == a->cnt) return 0;
    else if (cnt > a->cnt) return 1;
    return -1;
}
