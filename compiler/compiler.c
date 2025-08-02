#include <compiler.h>
#include <stdlib.h>

extern void abort_compilation(void*);

const char* get_runtime_header_path(void) {
    const char* header_path = getenv("COG_HEADER");

    if (!header_path) {
        fprintf(stderr, "Cog: Could not locate Runtime Header Path\n");
        fprintf(stderr, "      * please add 'COG_HEADER' to your environment, pointing to cog runtime header: 'cog.h'\n");
        abort_compilation(NULL);
    }
    
    return jarena_strdup(global_arena, (char*)header_path);
}
