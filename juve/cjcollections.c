#include <juve_utils.h>
#include <string.h>

#define CJVEC_INIT 255

struct cjvec_t {
    void** data;
    size_t cap;
    size_t len;
    juve_arena_t* arena;
};

cjvec_t* cjvec_new(juve_arena_t* arena) {
    cjvec_t* vec = (cjvec_t*)jarena_alloc(arena, sizeof(cjvec_t));
    if (!vec) return NULL;
    
    vec->cap = CJVEC_INIT;
    vec->len = 0;
    vec->arena = arena;

    vec->data = (void**)jarena_alloc(arena, sizeof(void*)*vec->cap);
    if (!vec->data) return NULL;
    
    return vec;
}

cjvec_t* cjvec_lines();

void cjvec_push(cjvec_t* vec, void* data) {

    if (vec->len >= vec->cap) {
        size_t newsz = vec->cap*2;
        void** new_data = (void**)jarena_alloc(vec->arena, newsz);
        
        while (vec->len >= vec->cap) {
            newsz = vec->cap*2;
            new_data = (void**)jarena_alloc(vec->arena, newsz);
        }
        
        memcpy(new_data, vec->data, vec->len);
        vec->data = new_data;
        vec->cap = newsz;
    }
    vec->data[vec->len++] = data;
}

size_t cjvec_len(cjvec_t* vec) {
    if (!vec) return -1;
    return vec->len;
}

void*  cjvec_at(cjvec_t* vec, size_t n) {
    if (!vec) return NULL;
    return vec->data[n];
}

void* cjvec_back(cjvec_t* vec) {
    if (!vec) return NULL;
    return vec->data[vec->len-1];    
}

void  cjvec_free(cjvec_t* vec) {
    // don't free anything.
    // structure is allocated on the arena
}

void  cjvec_free_all(cjvec_t* vec, void* user_data, cjvec_free_fn fn) {
    if (!vec) return;
    
    int max = cjvec_len(vec);
    for (int i = 0; i < max; i++) {
        void* data = cjvec_at(vec, i);
        if (data) {
            fn(user_data, data);
        }
    }
}
