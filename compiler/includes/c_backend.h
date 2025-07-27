#pragma once
#include <compiler.h>
#include <compiler_opt.h>
#include <stdint.h>

typedef struct CContext CContext;
typedef struct CFunction CFunction;
typedef struct CType CType;
typedef struct CValue CValue;

typedef enum {
    CStorageExtern,
    CStorageInline,
} CStorageAttr;

typedef enum {
    CTypeInt,
    CTypePointer,
    CTypeChar,
    CTypeDouble,
    CTypeVoid,
} CTypeKind;

typedef enum {
    CIntSigned,
    CIntUnsigned,
} CIntSign;

CContext*  cctx_new(JArena* arena);
CFunction* cctx_create_function(CContext* cctx, const char* name, CJVec* params, CJVec* sattr, CType* type);
void       cctx_end_function(CContext* cctx, CFunction* func, CValue* value);

// int 
CType*     cctx_create_type_int32(CContext* cctx, CIntSign sign, bool const_);
// char*, const char*
CType*     cctx_create_string_type(CContext* cctx, bool const_);

CValue*    cctx_create_value_int(CContext* ctx, CType* type, int64_t value);
CValue*    cctx_create_value_string(CContext* ctx, CType* type, const char* value);
CValue*    cctx_create_value_char(CContext* ctx, CType* type, char value);

JBuffer* cctx_get_output(CContext* ctx);
void cctx_free(CContext* ctx);

