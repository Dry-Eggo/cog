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
    Semantics* sema = alloc(Semantics);
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

void add_diagnostic(Semantics* sema, sema_error_t* err);

void stream_out(Semantics* sema, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char str[1024] = {0};
    vsprintf(str, fmt, args);
    
    jb_appendf_a(sema->tmp_out, global_arena, str);
    va_end(args);
}

void add_diagnostic(Semantics* sema, sema_error_t* err) {
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
    return cctx_create_type_int32(sema->cctx, CIntSigned, false);
}

void check_function(Semantics* sema, Item* item) {
    FunctionDef funcdef = item->data.fndef;
    TypeInfo* final_ty = NULL;
    if (funcdef.is_extern) stream_out(sema, "extern ");

    if (funcdef.return_type) {
        if (type_get_kind(funcdef.return_type) == type_any_k) {
            todo("funcdef.return_type == any");
        } else {        
            final_ty = get_type_info(sema, type_get_name(funcdef.return_type));
            if (!final_ty) {
                // ERROR_IMPLEMENTATION
                log_err("invalid type: '%s'\n", type_get_name(funcdef.return_type));
                todo("error_implementation");
            }
        }
    }
    
    if (strcmp(funcdef.name,"main") == 0) {
        final_ty = get_type_info(sema, "int");
    }

    CType* functype = sema_convert_type(sema, final_ty);    
    CFunction* func =  cctx_create_function(sema->cctx, funcdef.name, NULL, NULL, functype, true);
    
    if (funcdef.is_decl) stream_out(sema, ";");
    else {
        if (funcdef.body) {
            check_expr(sema, funcdef.body);
        }
    }
    
    cctx_end_function(sema->cctx, func, NULL);
}

StmtResult check_stmt(Semantics* sema, Stmt* stmt) {
    if (stmt->kind == stmt_vardecl_k) {
        VarDeclStmt vardecl = stmt->data.vardecl;
        ExprResult expr = check_expr(sema, vardecl.rhs);
        TypeInfo* final_ty = vardecl.type;
        
        if (type_get_kind(vardecl.type) == type_any_k) {
            if (!expr.type) {
                // ERROR_IMPLEMENTAION
                log_err("invalid inference\n");
                todo("error_implementation");
            }
            else {
                final_ty = get_type_info(sema, type_get_name(expr.type));
                if (!final_ty) {
                    // ERROR_IMPLEMENTAION
                    log_err("invalid type: '%s'\n", type_get_name(vardecl.type));
                    todo("error_implementation");                    
                }
            }
        } else {
            final_ty = get_type_info(sema, type_get_name(vardecl.type));
            if (!final_ty) {
                // ERROR_IMPLEMENTAION
                log_err("invalid type: '%s'\n", type_get_name(vardecl.type));
                todo("error_implementation");
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
        fori(Stmt*, stmt, stmt_count, block.statements) {
            StmtResult res = check_stmt(sema, stmt);
        }                
        return eresult_new(NULL, NULL);
    } break;
    case expr_int_k: {
        CType* int_type = cctx_create_type_int32(sema->cctx, CIntSigned, false);
        CExpr* cexpr = cctx_create_value_int(sema->cctx, int_type, expr->data.literal.lit.int_value);
        return eresult_new(cexpr, get_type_info(sema, "int"));
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
    default:
        todo("Semantics::check_expr::default");
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
    // todo: more structural comparison
    return type_get_kind(t1) == type_get_kind(t2);
}

TypeInfo*  check_type(Semantics* sema, TypeInfo* type) {

    if (type) {
        if (type_get_kind(type) == type_int_k) {
            TypeInfo* int_type = get_type_info(sema, "int");
            return int_type;
        }
    }
    todo("Semantics::check_type: add more types");
    return NULL;
}

CJVec* sema_get_diagnostics(Semantics* sema) {
    return sema->diagnostics;
}

bool sema_run_first_pass(Semantics* sema) {
    fori(Item*, item, item_count, sema->program) {
        if (item->kind == item_function_k) {
            add_func(sema, item->data.fndef);
        }        
    }        
    return true;
}

bool sema_run_second_pass(Semantics* sema) {
    fori(Item*, item, item_count, sema->program) {
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
