#include <juve/juve_utils.h>
#include <string.h>

#define CJVEC_INIT 255

struct CJVec {
    void** data;
    size_t cap;
    size_t len;
    JArena* arena;
};

CJVec* cjvec_new(JArena* arena) {
    CJVec* vec = (CJVec*)jarena_alloc(arena, sizeof(CJVec));
    if (!vec) return NULL;
    
    vec->cap = CJVEC_INIT;
    vec->len = 0;
    vec->arena = arena;

    vec->data = (void**)jarena_alloc(arena, sizeof(void*)*vec->cap);
    if (!vec->data) return NULL;
    
    return vec;
}

CJVec* cjvec_lines(void);

void cjvec_push(CJVec* vec, void* data) {

    if (vec->len >= vec->cap) {
        size_t newsz = vec->cap*2;
        void** new_data = (void**)jarena_alloc(vec->arena, newsz);       
        
        memcpy(new_data, vec->data, vec->len);
        vec->data = new_data;
        vec->cap = newsz;
    }
    vec->data[vec->len++] = data;
}

size_t cjvec_len(CJVec* vec) {
    if (!vec) return -1;
    return vec->len;
}

void*  cjvec_at(CJVec* vec, size_t n) {
    if (!vec) return NULL;
    return vec->data[n];
}

void* cjvec_back(CJVec* vec) {
    if (!vec) return NULL;
    return vec->data[vec->len-1];    
}

void  cjvec_free(CJVec* vec) {
    (void) vec;
    // don't free anything.
    // structure is allocated on the arena
}

