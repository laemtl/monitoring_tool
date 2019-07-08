#include <stdbool.h>

bool is_top(void** tl, int count, void* c, int (*compare_fn)(void*, void*)) {
    int i = 0;
    while (i < count) {
        if (compare_fn(c, tl[i]) == 0) return true;
        i++;
    }
    return false;
}

int sort_tl(void** tl, int n_index, int (*compare_fn)(void*, void*)) {
    //if(n_index == 0) return;
    
    int i = n_index;
    // Element to insert is stored at the last position
    void* e = tl[i];
    while (i > 0 && compare_fn(e, tl[i-1]) > 0) {   
        tl[i] = tl[i-1];
        i = i - 1;
    }
    tl[i] = e; 

    // Return the position where inserted.
    return i;
}