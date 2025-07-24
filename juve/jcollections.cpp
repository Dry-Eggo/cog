#include <juve_utils.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

struct juve_vec {
    std::vector<void*> data;
    juve_vec() {}
};

struct juve_map {
    std::unordered_map<std::string, void*> data;
    juve_map() {}
};

juve_vec_t* jvec_new() {
    return new juve_vec();
}

juve_vec_t* jvec_lines(const char* source, juve_arena_t* arena) {
    juve_vec_t* vec = jvec_new();
    std::stringstream ss;
    ss << source;
    std::string line;
    while (std::getline(ss, line)) {
	jvec_push(vec, (void*)jarena_strdup(arena, (char*)line.c_str()));
    }
    return vec;
}

void jvec_push(juve_vec_t* vec, void* ptr) {
    if (!vec || !ptr) return;
    vec->data.push_back(ptr);
}

size_t jvec_len(juve_vec_t* vec) {
    if (!vec) return -1;
    return vec->data.size();
}

void*  jvec_at(juve_vec_t* vec, size_t n) {
    if (!vec) return nullptr;
    if (n >= vec->data.size()) return nullptr;
    return vec->data.at(n);
}

void* jvec_back(juve_vec_t* vec) {
    if (!vec) return nullptr;
    return vec->data.back();
}

void  jvec_free(juve_vec_t* vec) {
    if(!vec) return;
    delete vec;
}

juve_map_t* jmap_new() {
    return new juve_map();
}

void jmap_put(juve_map_t* map, const char* k, void* v) {
    if (!map || !k || !v) return;
    map->data[k] = v;    
}

int jmap_has(juve_map_t* map, const char* k)  {
    if (!map || !k) return -1;
    return map->data.count(k) != 0;
}

void* jmap_get(juve_map_t* map,  const char* k) {
    if (!map || !k) return nullptr;
    return map->data[k];
}

void jmap_free(juve_map_t* map) {
    if (!map) return;
    delete map;
}
