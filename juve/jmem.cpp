#include <juve_utils.h>
#include <vector>
#include <cstring>

void* jmalloc(size_t n) {
    return ::operator new(n);
}

void jfree(void* ptr) {
    ::operator delete(ptr);
}

struct juve_arena {
    std::vector<void*> allocations;
    juve_arena() {}
};

juve_arena_t* jarena_new() {
    return new juve_arena();
}

void* jarena_alloc(juve_arena_t* arena, size_t n) {
    if (!arena) return nullptr;
    void* mem = jmalloc(n);
    if (mem) {
	arena->allocations.push_back(mem);
	return mem;
    }
    return nullptr;
}

void* jarena_zeroed(juve_arena_t* arena, size_t n) {
    if (!arena) return nullptr;
    void* mem = jmalloc(n);
    if (mem) {
	memset(mem, 0, n);
	arena->allocations.push_back(mem);
	return mem;
    }
    return nullptr;    
}

char* jarena_strdup(juve_arena_t* arena, char* str) {
    if (!arena || !str) return nullptr;
    size_t str_len = strlen(str) + 1;
    char* new_str = (char*)jarena_alloc(arena, str_len);
    memcpy(new_str, str, str_len);
    new_str[str_len] = '\0';
    return new_str;
}

void jarena_reset(juve_arena_t* arena) {
    if (!arena) return;
    for (void* mem: arena->allocations) jfree(mem);
    arena->allocations.clear();
}

void jarena_free(juve_arena_t* arena) {
    if (!arena) return;
    for (void* mem: arena->allocations) jfree(mem);
    delete arena;
}
