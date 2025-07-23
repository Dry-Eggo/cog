#include <juve_utils.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>

struct juve_buffer {
    std::stringstream buffer;
    juve_buffer() {
    }
};

juve_buffer* jb_create() {
    return new juve_buffer();
}

void jb_print(juve_buffer* jb) {
    if (!jb) return;
    printf("%s", jb->buffer.str().c_str());
}

void jb_append(juve_buffer* jb, const char* str) {
    if (!jb) return;
    jb->buffer << str;
}

char* jb_str(juve_buffer* jb) {
    if (!jb) return nullptr;
    std::string content = jb->buffer.str();
    char* res = static_cast<char*>(::operator new(content.size() + 1));
    std::memcpy(res, content.c_str(), content.size() + 1);
    return res;
}

void jb_clear(juve_buffer* jb) {
    if (!jb) return;

    jb->buffer.str("");
    jb->buffer.clear();
}

void jb_free(juve_buffer* jb) {
    delete jb;
}
