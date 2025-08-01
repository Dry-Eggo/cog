#ifndef __COG_HEADER_H__
#define __COG_HEADER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int8_t   cog_i8;
typedef int16_t  cog_i16;
typedef int32_t  cog_i32;
typedef int64_t  cog_i64;
typedef cog_i32 cog_int;

typedef uint8_t  cog_u8;
typedef uint16_t cog_u16;
typedef uint32_t cog_u32;
typedef uint64_t cog_u64;

typedef struct cog_string cog_string;
typedef const char* cog_cstr;
typedef void*       cog_ptr;

#define cog_nil NULL;

#endif __COG_HEADER_H__
