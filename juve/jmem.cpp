#include <juve_utils.h>


void* jmalloc(size_t n) {
    return ::operator new(n);
}

void jfree(void* ptr) {
    ::operator delete(ptr);
}
