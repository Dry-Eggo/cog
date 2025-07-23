#pragma once

#ifdef __cplusplus
#include <stddef.h>
#include <stdbool.h>
extern "C" {
#endif

typedef struct juve_buffer juve_buffer;

juve_buffer* jb_create();
void jb_append(juve_buffer*, const char*);
void jb_print(juve_buffer*);
char* jb_str(juve_buffer*);
void jb_free(juve_buffer*);
void jb_clear(juve_buffer*);

// Memory utils
void* jmalloc(size_t);
void  jfree(void*);

// file utils
size_t jb_read_entire_file(const char*, juve_buffer*);

#ifdef __cplusplus
}
#endif
