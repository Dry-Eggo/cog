#pragma once
#include <compiler.h>
#include <compiler_opt.h>
#include <stdint.h>

typedef struct CContext    CContext;
typedef struct CParam      CParam;
typedef struct CFunction   CFunction;
typedef struct CExpr       CExpr;
typedef struct CStmt       CStmt;

typedef enum {
    CStorageExtern,
    CStorageInline,
} CStorageAttr;

typedef enum {
    CBinaryAdd,
    CBinarySub,
    CBinaryMul,
    CBinaryDiv,
} CBinaryOp;

CContext*  cctx_new(JArena* arena);

void       cctx_include(CContext* cctx, const char* path, bool system);

CParam*    cctx_new_parameter(CContext* cctx, const char* name, const char* type);

CFunction* cctx_create_function(CContext* cctx, const char* name, CJVec* params, const char* type, bool has_definition);
void       cctx_function_set_extern(CContext* cctx, CFunction* func);
void       cctx_function_set_variadic(CContext* cctx, CFunction* func);
void       cctx_end_function(CContext* cctx, CFunction* func, CExpr* value);

CExpr*    cctx_create_value_int(CContext* ctx, const char* type, int64_t value);
CExpr*    cctx_create_value_string(CContext* ctx, const char* type, const char* value);
CExpr*    cctx_create_value_char(CContext* ctx, const char* type, char value);
CExpr*    cctx_create_value_identifer(CContext* cctx, const char* identifier);
CExpr*    cctx_create_binop_expr(CContext* cctx, CExpr* lhs, CExpr* rhs, CBinaryOp op);
CExpr*    cctx_sub_expr(CContext* cctx, CExpr* lhs, CExpr* rhs);
CExpr*    cctx_call(CContext* cctx, const char* who, CJVec* args);

void cctx_assign_value(CContext* cctx, const char* type, const char* name, CExpr* expr);
void cctx_terminate_expr(CContext* cctx, CExpr* expr);

CJBuffer* cctx_get_output(CContext* ctx);
void cctx_free(CContext* ctx);

