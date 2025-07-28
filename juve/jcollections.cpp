#include <juve/juve_utils.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

struct JVec {
    std::vector<void*> data;
    JVec() {}
};

struct JMap {
    std::unordered_map<std::string, void*> data;
    JMap() {}
};

JVec* jvec_new() {
    return new JVec();
}

JVec* jvec_lines(const char* source, JArena* arena) {
    JVec* vec = jvec_new();
    std::stringstream ss;
    ss << source;
    std::string line;
    while (std::getline(ss, line)) {
	    jvec_push(vec, (void*)jarena_strdup(arena, (char*)line.c_str()));
    }
    return vec;
}

void jvec_push(JVec* vec, void* ptr) {
    if (!vec || !ptr) return;
    vec->data.push_back(ptr);
}

size_t jvec_len(JVec* vec) {
    if (!vec) return -1;
    return vec->data.size();
}

void*  jvec_at(JVec* vec, size_t n) {
    if (!vec) return nullptr;
    if (n >= vec->data.size()) return nullptr;
    return vec->data.at(n);
}

void* jvec_back(JVec* vec) {
    if (!vec) return nullptr;
    return vec->data.back();
}

void  jvec_free(JVec* vec) {
    if(!vec) return;
    delete vec;
}

JMap* jmap_new() {
    return new JMap();
}

void jmap_put(JMap* map, const char* k, void* v) {
    if (!map || !k || !v) return;
    map->data[k] = v;    
}

int jmap_has(JMap* map, const char* k)  {
    if (!map || !k) return -1;
    return map->data.count(k) != 0;
}

void* jmap_get(JMap* map,  const char* k) {
    if (!map || !k) return nullptr;
    return map->data[k];
}

void jmap_free(JMap* map) {
    if (!map) return;
    delete map;
}
