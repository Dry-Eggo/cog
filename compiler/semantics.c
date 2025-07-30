#include <semantics.h>
#include <errors.h>
#include <variables.h>
#include <defines.h>
#include <stdlib.h>
#include <nodes.h>
#include <functions.h>
#include <string.h>
#include <stdarg.h>
#include <c_backend.h>

struct Semantics {
    CContext* cctx;
    CJVec* program;
    CJVec* diagnostics;
    JVec* source_lines;

    JMap* functions;
    JMap* types;
    JMap* symbols;
    
    CompileOptions* options;

    const char* source;

    // for temporar visualizaton of the generated code
    // will be delegated to a proper backend dispatcher
    JBuffer* tmp_out;
};

JBuffer* sema_get_tmp(Semantics* sema) {
    return sema->tmp_out;
}

typedef struct {
    CExpr*        expr;
    TypeInfo*     type;
} ExprResult;

typedef struct {
    CStmt* stmt;
} StmtResult;

StmtResult sresult_new(CStmt* s) {
    return (StmtResult) { s };
}

ExprResult eresult_new(CExpr* e, TypeInfo* ty) {
    return (ExprResult) { e, ty };
}


Semantics* semantics_init(CJVec* items, JVec* source_lines, const char* source, CompileOptions* opts) {
    Semantics* sema = ALLOC(Semantics);
    sema->program = items;
    sema->source_lines = source_lines;
    sema->source = source;
    sema->options = opts;

    sema->cctx = cctx_new(global_arena);
    
    sema->diagnostics = cjvec_new(global_arena);
    sema->tmp_out = jb_create();
    
    sema->functions = jmap_new();
    sema->symbols   = jmap_new();
    sema->types = jmap_new();


    // initialize builtin types
    // todo: move to a seperate function
    jmap_put(sema->types, "int", type_new(type_int_k, "int", "int"));
    jmap_put(sema->types, "cstr", type_new(type_cstring_k, "cstr", "const char*"));
    jmap_put(sema->types, "none", type_new(type_none_k, "none", "void"));
    
    return sema;
}

CContext* sema_get_cctx(Semantics* sema) {
    return sema->cctx;
}

void sema_free(Semantics* sema) {
    if (sema) {
        jmap_free(sema->types);
        jmap_free(sema->functions);
        jmap_free(sema->symbols);
        jb_free(sema->tmp_out);
        cctx_free(sema->cctx);
    }
}

void register_sym(Semantics* sema, const char*, SymInfo* sym);
SymInfo* get_syminfo(Semantics* sema, const char* identifer);
void check_function(Semantics* sema, Item* item);
ExprResult check_expr(Semantics* sema, Expr* expr);
StmtResult check_stmt(Semantics* sema, Stmt* stmt);

// returns stored types out of parsed ones
TypeInfo* check_type(Semantics* sema, TypeInfo* type);
TypeInfo* get_type_info(Semantics* sema, const char* type_name);
bool type_match(Semantics* sema, TypeInfo* t1, TypeInfo* t2);

// cbackend
CType* sema_convert_type(Semantics* sema, TypeInfo* ty);

void add_diagnostic(Semantics* sema, SemaError* err);

void stream_out(Semantics* sema, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char str[1024] = {0};
    vsprintf(str, fmt, args);
    
    jb_appendf_a(sema->tmp_out, global_arena, str);
    va_end(args);
}

void add_diagnostic(Semantics* sema, SemaError* err) {
    cjvec_push(sema->diagnostics, (void*)err);
}

void add_func(Semantics* sema, FunctionDef func) {
    FunctionInfo* finfo = func_info_new(func.name, func.linkage_name, func.name, NULL, func.return_type);
    jmap_put(sema->functions, func.name, (void*)finfo);
}

void add_type(Semantics* sema, const char* name, TypeInfo* type) {
    jmap_put(sema->types, name, (void*)type);
}

void register_sym(Semantics* sema, const char* name, SymInfo* sym) {
    jmap_put(sema->symbols, name, (void*)sym);
}

SymInfo* get_syminfo(Semantics* sema, const char* identifer) {
    return jmap_get(sema->symbols, identifer);
}

CType* sema_convert_type(Semantics* sema, TypeInfo* ty) {
    if (type_get_kind(ty) == type_int_k) return cctx_create_type_int32(sema->cctx, CIntSigned, false);
    if (type_get_kind(ty) == type_cstring_k) return cctx_create_type_string(sema->cctx, true);
    return NULL;
}

CJVec* sema_convert_param(Semantics* sema, Params params) {
    CJVec* vec = cjvec_new(global_arena);
    size_t max = params.count;
    for (size_t i = 0; i < max; ++i) {
        ParamDef paramdef = params.items[i];
        CType*   type     = sema_convert_type(sema, paramdef.type);
        cjvec_push(vec, cctx_new_parameter(sema->cctx, paramdef.name, type));
    }
    
    return vec;
}

void check_function(Semantics* sema, Item* item) {
    FunctionDef funcdef = item->data.fndef;
    TypeInfo* final_ty = NULL;

    if (funcdef.return_type) {
        if (type_get_kind(funcdef.return_type) == type_any_k) {
            TODO("funcdef.return_type == any");
        } else {        
            final_ty = get_type_info(sema, type_get_name(funcdef.return_type));
            if (!final_ty) {
                // ERROR_IMPLEMENTATION
                LOG_ERR("invalid type: '%s'\n", type_get_name(funcdef.return_type));
                TODO("error_implementation");
            }
        }
    }
    
    if (strcmp(funcdef.name,"main") == 0) {
        final_ty = get_type_info(sema, "int");
    }
    
    CType* functype = sema_convert_type(sema, final_ty);    
    CFunction* func = NULL;

    CJVec* params = sema_convert_param(sema, funcdef.params);
    
    if (funcdef.is_decl) {
        func =  cctx_create_function(sema->cctx, funcdef.name, params, functype, false);        
    }
    else {
        func =  cctx_create_function(sema->cctx, funcdef.name, params, functype, true);
        check_expr(sema, funcdef.body);
    }
    
    if (funcdef.is_extern) cctx_function_set_extern(sema->cctx, func);
    if (funcdef.is_variadic) cctx_function_set_variadic(sema->cctx, func);
    
    cctx_end_function(sema->cctx, func, NULL);
}

StmtResult check_stmt(Semantics* sema, Stmt* stmt) {
    if (stmt->kind == stmt_vardecl_k) {
        VarDeclStmt vardecl = stmt->data.vardecl;
        ExprResult expr = check_expr(sema, vardecl.rhs);

        if (!expr.expr) {
            // An error occured.
            // we should not continue further.
            // Diagnostics will have already been generated
            return sresult_new(NULL);
        }
        
        TypeInfo* final_ty = vardecl.type;
        
        if (type_get_kind(vardecl.type) == type_any_k) {
            if (!expr.type) {
                // ERROR_IMPLEMENTAION
                LOG_ERR("invalid inference\n");
                TODO("error_implementation");
            }
            else {
                final_ty = get_type_info(sema, type_get_name(expr.type));
                if (!final_ty) {
                    // ERROR_IMPLEMENTAION
                    LOG_ERR("invalid type: '%s'\n", type_get_name(vardecl.type));
                    TODO("error_implementation");                    
                }
            }
        } else {
            final_ty = get_type_info(sema, type_get_name(vardecl.type));
            if (!final_ty) {
                // ERROR_IMPLEMENTAION
                LOG_ERR("invalid type: '%s'\n", type_get_name(vardecl.type));
                TODO("error_implementation");
            }
            
            if (!type_match(sema, final_ty, expr.type)) {
                add_diagnostic(sema, make_invalid_type(vardecl.rhs->span, type_get_name(expr.type), type_get_name(final_ty)));
                return sresult_new(NULL);
            }
        }

        CType* var_type = sema_convert_type(sema, final_ty);
        cctx_assign_value(sema->cctx, var_type, vardecl.identifer, expr.expr);
        register_sym(sema, vardecl.identifer, syminfo_new(vardecl.identifer, vardecl.span, final_ty, sym_variable_k));
    }
    return sresult_new(NULL);
}

ExprResult check_expr(Semantics* sema, Expr* expr) {
    switch(expr->kind) {
    case expr_compound_stmt_k: {
        BlockExpr block = expr->data.block;
        FOREACH(Stmt*, stmt, stmt_count, block.statements) {
            check_stmt(sema, stmt);
        }                
        return eresult_new(NULL, NULL);
    } break;
    case expr_int_k: {
        CType* int_type = cctx_create_type_int32(sema->cctx, CIntSigned, false);
        CExpr* cexpr = cctx_create_value_int(sema->cctx, int_type, expr->data.literal.lit.int_value);
        return eresult_new(cexpr, get_type_info(sema, "int"));
    } break;
    case expr_cstring_k: {
        CType* cstr_type = cctx_create_type_string(sema->cctx, true);
        CExpr* cexpr = cctx_create_value_string(sema->cctx, cstr_type, expr->data.literal.lit.cstring_value);
        return eresult_new(cexpr, get_type_info(sema, "cstr"));
    } break;
    case expr_ident_k: {
        const char* identifer = expr->data.literal.lit.str_value;
        SymInfo* syminfo = get_syminfo(sema, identifer);
        if (!syminfo) {
            add_diagnostic(sema, make_undeclared_var(expr->span, identifer));
            return eresult_new(NULL, get_type_info(sema, "int"));   
        }
        TypeInfo* var_type = syminfo_get_type(syminfo);
        return eresult_new(cctx_create_value_identifer(sema->cctx, identifer), get_type_info(sema, type_get_name(var_type)));
    } break;
    case expr_binop_k: {
        BinaryOpExpr binop = expr->data.binop;
        ExprResult lhs_expr = check_expr(sema, binop.lhs);
        ExprResult rhs_expr = check_expr(sema, binop.rhs);

        ASSERT(lhs_expr.type != NULL, "lhs_type is null");
        ASSERT(rhs_expr.type != NULL, "rhs_type is null");

        const char* lhs_ty = type_get_name(lhs_expr.type);
        const char* rhs_ty = type_get_name(rhs_expr.type);
        
        if (!type_match(sema, lhs_expr.type, get_type_info(sema, "int"))) {
            add_diagnostic(sema, make_invalid_binary_operand(binop.lhs->span, lhs_ty));  
        }

        if (!type_match(sema, rhs_expr.type, get_type_info(sema, "int"))) {
            add_diagnostic(sema, make_invalid_binary_operand(binop.rhs->span, rhs_ty));  
        }
        
        if (!type_match(sema, lhs_expr.type, rhs_expr.type)) {

            // In the future, BinaryOp Interface may be implemented to support operator overloading
            // But for now. this is an invalid operation           
            add_diagnostic(sema, make_invalid_operand_type(binop.lhs->span, binop.rhs->span, lhs_ty, rhs_ty));
            return eresult_new(NULL, lhs_expr.type);
        }
        
        switch (binop.op) {
        case BINOP_ADD: return eresult_new(cctx_add_expr(sema->cctx, lhs_expr.expr, rhs_expr.expr), lhs_expr.type);
        case BINOP_SUB: return eresult_new(cctx_sub_expr(sema->cctx, lhs_expr.expr, rhs_expr.expr), lhs_expr.type);
        default: TODO("Add all operators");
        }
        
    } break;
    default:
        TODO("Semantics::check_expr::default");
        break;
    }
    return eresult_new(NULL, NULL);
}

TypeInfo* get_type_info(Semantics* sema, const char* type_name) {
    if (!jmap_has(sema->types, type_name)) {
        return NULL;
    }
    return jmap_get(sema->types, type_name);
}

bool type_match(Semantics* sema, TypeInfo* t1, TypeInfo* t2) {
    UNUSED(sema);
    // TODO: more structural comparison
    return type_get_kind(t1) == type_get_kind(t2);
}

TypeInfo*  check_type(Semantics* sema, TypeInfo* type) {

    if (type) {
        if (type_get_kind(type) == type_int_k) {
            TypeInfo* int_type = get_type_info(sema, "int");
            return int_type;
        }
    }
    TODO("Semantics::check_type: add more types");
    return NULL;
}

CJVec* sema_get_diagnostics(Semantics* sema) {
    return sema->diagnostics;
}

bool sema_run_first_pass(Semantics* sema) {
    FOREACH(Item*, item, item_count, sema->program) {
        if (item->kind == item_function_k) {
            add_func(sema, item->data.fndef);
        }        
    }        
    return true;
}

bool sema_run_second_pass(Semantics* sema) {
    FOREACH(Item*, item, item_count, sema->program) {
        if (item->kind == item_function_k) {
            check_function(sema, item);
        }        
    }    
    return true;
}

bool sema_check(Semantics* sema) {        
    if (!sema_run_first_pass(sema)) return false;
    if (!sema_run_second_pass(sema)) return false;
    return cjvec_len(sema->diagnostics) == 0;
}
