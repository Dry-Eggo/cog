#include <juve_utils.h>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>

size_t jb_read_entire_file(const char* path, juve_buffer* jb) {
    std::fstream file(path);

    if (!file.is_open()) return -1;
    
    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    jb_append(jb, content.c_str());
    return ss.str().size();
}
