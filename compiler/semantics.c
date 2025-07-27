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
    CContext* ccontext;
    CJVec* program;
    CJVec* diagnostics;
    JVec* source_lines;

    juve_map_t* functions;
    juve_map_t* types;
    juve_map_t* symbols;
    
    CompileOptions* options;

    const char* source;

    // for temporar visualizaton of the generated code
    // will be delegated to a proper backend dispatcher
    juve_buffer_t* tmp_out;
};

juve_buffer_t* sema_get_tmp(Semantics* sema) {
    return sema->tmp_out;
}

typedef struct {
    const char* result;
    const char* preamble;

    TypeInfo*     type;
} expr_result_t;

typedef struct {
    const char* result;    
} stmt_result_t;

stmt_result_t sresult_new(const char* s) {
    return (stmt_result_t) { s };
}

expr_result_t eresult_new(const char* s, const char* p, TypeInfo* ty) {
    return (expr_result_t) { s, p, ty, };
}


Semantics* semantics_init(CJVec* items, JVec* source_lines, const char* source, CompileOptions* opts) {
    Semantics* sema = alloc(Semantics);
    sema->program = items;
    sema->source_lines = source_lines;
    sema->source = source;
    sema->options = opts;

    sema->ccontext = cctx_new(global_arena);
    
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
    return sema->ccontext;
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
expr_result_t check_expr(Semantics* sema, Expr* expr);
stmt_result_t check_stmt(Semantics* sema, Stmt* stmt);

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
    return cctx_create_type_int32(sema->ccontext, CIntSigned, false);
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
    CFunction* func =  cctx_create_function(sema->ccontext, funcdef.name, NULL, NULL, functype);
    
    stream_out(sema, "%s %s ()\n", type_get_repr(final_ty), funcdef.name);
    if (funcdef.is_decl) stream_out(sema, ";");
    else {
        stream_out(sema, "{");
        if (funcdef.body) {
            expr_result_t body_code = check_expr(sema, funcdef.body);
            if (body_code.result) {
                stream_out(sema, "%s", body_code.result);
            }
        }
        stream_out(sema, "\n}\n");
    }
    // cctx_end_function(sema->ccontext, func, NULL);
}

stmt_result_t check_stmt(Semantics* sema, Stmt* stmt) {
    juve_buffer_t* code = jb_create();
    if (stmt->kind == stmt_vardecl_k) {
        VarDeclStmt vardecl = stmt->data.vardecl;
        expr_result_t expr = check_expr(sema, vardecl.rhs);
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
        
        jb_appendf_a(code, global_arena, "\n\t%s %s = %s;", type_get_repr(final_ty), vardecl.identifer, expr.result);
        register_sym(sema, vardecl.identifer, syminfo_new(vardecl.identifer, vardecl.span, final_ty, sym_variable_k));
    }
    const char* res = jb_str_a(code, global_arena);
    jb_free(code);
    return sresult_new(res);
}

expr_result_t check_expr(Semantics* sema, Expr* expr) {
    juve_buffer_t* code = jb_create();
    switch(expr->kind) {
    case expr_compound_stmt_k: {
        BlockExpr block = expr->data.block;
        fori(Stmt*, stmt, stmt_count, block.statements) {
            stmt_result_t res = check_stmt(sema, stmt);
            jb_appendf_a(code, global_arena, "%s", res.result);
        }        
        const char* res = jb_str_a(code, global_arena);
        
        jb_free(code);
        return eresult_new(res, NULL, NULL);
    } break;
    case expr_int_k: {
        jb_appendf_a(code, global_arena, "%ld", expr->data.literal.lit.int_value);
        const char* res = jb_str_a(code, global_arena);
        jb_free(code);
        return eresult_new(res, NULL, get_type_info(sema, "int"));
    } break;
    case expr_ident_k: {
        const char* identifer = expr->data.literal.lit.str_value;
        jb_appendf_a(code, global_arena, "%s", identifer);
        const char* res = jb_str_a(code, global_arena);
        jb_free(code);

        SymInfo* syminfo = get_syminfo(sema, identifer);
        if (!syminfo) {
            add_diagnostic(sema, make_undeclared_var(expr->span, identifer));
            return eresult_new(res, NULL, get_type_info(sema, "int"));   
        }
        TypeInfo* var_type = syminfo_get_type(syminfo);
        return eresult_new(res, NULL, get_type_info(sema, type_get_name(var_type)));
    } break;    
    default:
        todo("Semantics::check_expr::default");
        break;
    }
    return eresult_new(NULL, NULL, NULL);
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
