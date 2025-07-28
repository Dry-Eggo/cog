#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct JBuffer JBuffer;
typedef struct JArena JArena;

// Memory utils
void* jmalloc(size_t);
void  jfree(void*);

JArena* jarena_new(void);
void* jarena_alloc(JArena*, size_t);
void* jarena_zeroed(JArena*, size_t);
void jarena_free(JArena*);
void jarena_reset(JArena*);
char* jarena_strdup(JArena*, char*);

// buffer
JBuffer* jb_create(void);
void jb_append(JBuffer*, const char*);
void jb_appendf_a(JBuffer*, JArena*, const char* fmt, ...);
void jb_print(JBuffer*);
char* jb_str(JBuffer*);
char* jb_str_a(JBuffer* jb, JArena* arena);
void jb_free(JBuffer*);
void jb_clear(JBuffer*);
bool jb_eq(JBuffer*, const char*);
size_t jb_len(JBuffer*);

// file utils
size_t jb_read_entire_file(const char*, JBuffer*);
bool jfile_write(const char*, const char*);
char*  jfile_stem(const char*, JArena*);
char*  jfile_ext(const char*, JArena*);
char* jfile_dirname(const char*, JArena*);
char* jfile_basename(const char*, JArena*);
bool jfile_exists(const char*);
long jfile_size(const char*);

// containers
typedef struct JMap JMap;
typedef struct JVec JVec;
JVec* jvec_new(void);
JVec* jvec_lines(const char* source, JArena*);
void jvec_push(JVec*, void*);
size_t jvec_len(JVec*);
void*  jvec_at(JVec*, size_t);
void* jvec_back(JVec*);
void  jvec_free(JVec*);

JMap* jmap_new(void);
void jmap_put(JMap*, const char*, void*);
int jmap_has(JMap*, const char*);
void* jmap_get(JMap*, const char*);
void jmap_free(JMap*);

// c-containers
typedef struct CJVec CJVec;

CJVec* cjvec_new(JArena* arena);
CJVec* cjvec_lines(void);
void cjvec_push(CJVec* vec, void* data);
size_t cjvec_len(CJVec*);
void*  cjvec_at(CJVec*, size_t);
void* cjvec_back(CJVec*);
void  cjvec_free(CJVec*);


#ifdef __cplusplus
}
#endif
