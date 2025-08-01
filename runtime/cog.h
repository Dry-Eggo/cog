#ifndef __COG_HEADER_H__
#define __COG_HEADER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef int8_t   kudo_i8;
typedef int16_t  kudo_i16;
typedef int32_t  kudo_i32;
typedef int64_t  kudo_i64;
typedef kudo_i32 kudo_int;

typedef uint8_t  kudo_u8;
typedef uint16_t kudo_u16;
typedef uint32_t kudo_u32;
typedef uint64_t kudo_u64;

typedef struct kudo_string kudo_string;
typedef const char* kudo_cstr;
typedef void*       kudo_ptr;

#define kudo_nil NULL;

#endif __COG_HEADER_H__
