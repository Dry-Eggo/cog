#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct juve_buffer juve_buffer_t;
typedef struct juve_arena juve_arena_t;

// Memory utils
void* jmalloc(size_t);
void  jfree(void*);

juve_arena_t* jarena_new();
void* jarena_alloc(juve_arena_t*, size_t);
void* jarena_zeroed(juve_arena_t*, size_t);
void jarena_free(juve_arena_t*);
void jarena_reset(juve_arena_t*);
char* jarena_strdup(juve_arena_t*, char*);

// buffer
juve_buffer_t* jb_create();
void jb_append(juve_buffer_t*, const char*);
void jb_appendf_a(juve_buffer_t*, juve_arena_t*, const char* fmt, ...);
void jb_print(juve_buffer_t*);
char* jb_str(juve_buffer_t*);
char* jb_str_a(juve_buffer_t* jb, juve_arena_t* arena);
void jb_free(juve_buffer_t*);
void jb_clear(juve_buffer_t*);
bool jb_eq(juve_buffer_t*, const char*);

// file utils
size_t jb_read_entire_file(const char*, juve_buffer_t*);
bool jfile_write(const char*, const char*);
char*  jfile_stem(const char*, juve_arena_t*);
char*  jfile_ext(const char*, juve_arena_t*);
char* jfile_dirname(const char*, juve_arena_t*);
char* jfile_basename(const char*, juve_arena_t*);
bool jfile_exists(const char*);
long jfile_size(const char*);


// containers
typedef struct juve_map juve_map_t;
typedef struct juve_vec juve_vec_t;
juve_vec_t* jvec_new();
juve_vec_t* jvec_lines(const char* source, juve_arena_t*);
void jvec_push(juve_vec_t*, void*);
size_t jvec_len(juve_vec_t*);
void*  jvec_at(juve_vec_t*, size_t);
void* jvec_back(juve_vec_t*);
void  jvec_free(juve_vec_t*);

juve_map_t* jmap_new();
void jmap_put(juve_map_t*, const char*, void*);
int jmap_has(juve_map_t*, const char*);
void* jmap_get(juve_map_t*, const char*);
void jmap_free(juve_map_t*);

// c-containers
typedef struct cjvec_t cjvec_t;
typedef void (*cjvec_free_fn)(void*, void*);
cjvec_t* cjvec_new(juve_arena_t* arena);
cjvec_t* cjvec_lines();
void cjvec_push(cjvec_t* vec, void* data);
size_t cjvec_len(cjvec_t*);
void*  cjvec_at(cjvec_t*, size_t);
void* cjvec_back(cjvec_t*);
void  cjvec_free(cjvec_t*);
void  cjvec_free_all(cjvec_t* vec, void* user_data, cjvec_free_fn fn);

// cli
typedef struct jcli_args jcli_args_t;
void jcli_parse(int, char**, jcli_args_t*, juve_arena_t*);
jcli_args_t* jcli_new_a(juve_arena_t*);
void jcli_args_free(jcli_args_t*);
// checks if the args contains any flag named 'name'
// do not pass 'name' containing '--' or '-' again to this function
bool jcli_has_flag(jcli_args_t*, const char* name);
bool jcli_get_option(jcli_args_t*, const char* name, char** out);
bool jcli_has_flag_sl(jcli_args_t*, const char* short_n, const char* long_n);
bool jcli_get_option_sl(jcli_args_t*, const char* short_n, const char* long_n, char** out);

#ifdef __cplusplus
}
#endif
