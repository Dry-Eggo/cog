#pragma once
#include <compiler.h>
#include <compiler_opt.h>
#include <stdint.h>

typedef struct CContext CContext;
typedef struct CFunction CFunction;
typedef struct CType CType;
typedef struct CExpr CExpr;
typedef struct CStmt CStmt;

typedef enum {
    CStorageExtern,
    CStorageInline,
} CStorageAttr;

typedef enum {
    CTypeInt,
    CTypeString,
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
CFunction* cctx_create_function(CContext* cctx, const char* name, CJVec* params, CJVec* sattr, CType* type, bool has_definition);
void       cctx_end_function(CContext* cctx, CFunction* func, CExpr* value);

// int 
CType*     cctx_create_type_int32(CContext* cctx, CIntSign sign, bool const_);
// char*, const char*
CType*     cctx_create_type_string(CContext* cctx, bool const_);

CExpr*    cctx_create_value_int(CContext* ctx, CType* type, int64_t value);
CExpr*    cctx_create_value_string(CContext* ctx, CType* type, const char* value);
CExpr*    cctx_create_value_char(CContext* ctx, CType* type, char value);
CExpr*    cctx_create_value_identifer(CContext* cctx, const char* identifier);
CExpr*    cctx_add_expr(CContext* cctx, CExpr* lhs, CExpr* rhs);
CExpr*    cctx_sub_expr(CContext* cctx, CExpr* lhs, CExpr* rhs);

void cctx_assign_value(CContext* cctx, CType* type, const char* name, CExpr* expr);

JBuffer* cctx_get_output(CContext* ctx);
void cctx_free(CContext* ctx);

