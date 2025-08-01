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
#include <context.h>

struct Semantics {
    CContext*       cctx;
    CJVec*          program;
    CJVec*          diagnostics;
    JVec*           source_lines;
    Context*        root;
    Context*        current_context;    
    CompileOptions* options;
    const char*     source;
};

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

void add_type(Semantics* sema, const char* name, TypeInfo* type);

void initialize_builtin_types(Semantics* sema) {
    add_type(sema, "int",  type_new(type_int_k,     "int",  "kudo_int"));
    add_type(sema, "cstr", type_new(type_cstring_k, "cstr", "kudo_cstr"));
    add_type(sema, "none", type_new(type_none_k,    "none", "void"));
}

Semantics* semantics_init(CJVec* items, JVec* source_lines, const char* source, CompileOptions* opts) {
    Semantics* sema       = ALLOC(Semantics);
    sema->program         = items;
    sema->source_lines    = source_lines;
    sema->source          = source;
    sema->options         = opts;
    sema->cctx            = cctx_new(global_arena);    
    sema->diagnostics     = cjvec_new(global_arena);    
    sema->root            = create_new_context(NULL);
    sema->current_context = sema->root;
    initialize_builtin_types(sema);

    //  include the runtime header
    cctx_include(sema->cctx, get_runtime_header_path(), false);
    
    return sema;
}

CContext* sema_get_cctx(Semantics* sema) {
    return sema->cctx;
}

void sema_free(Semantics* sema) {
    if (sema) {
        if (sema->root == sema->current_context) {
            context_free(sema->root);
        } else {
            context_free(sema->root);
            context_free(sema->current_context);
        }        
        cctx_free(sema->cctx);
    }
}

void       register_sym(Semantics* sema, const char*, SymInfo* sym);
SymInfo*   get_syminfo(Semantics* sema, const char* identifer);
void       check_function(Semantics* sema, Item* item);
ExprResult check_expr(Semantics* sema, Expr* expr);
StmtResult check_stmt(Semantics* sema, Stmt* stmt);

// returns stored types out of parsed ones
TypeInfo* check_type(Semantics* sema, TypeInfo* type);
TypeInfo* get_type_info(Semantics* sema, const char* type_name);
bool      type_match(Semantics* sema, TypeInfo* t1, TypeInfo* t2);

void add_diagnostic(Semantics* sema, SemaError* err);

void enter_new_context(Semantics* sema);
void leave_context(Semantics* sema);

void add_diagnostic(Semantics* sema, SemaError* err) {
    cjvec_push(sema->diagnostics, (void*)err);
}

void add_func(Semantics* sema, FunctionDef func, Span span) {

    CJVec* params = cjvec_new(global_arena);
    for (size_t i = 0; i < func.params.count; ++i) {
        ParamDef pdef   = func.params.items[i];
        TypeInfo* ptype = check_type(sema, pdef.type);
        cjvec_push(params, param_info_new(pdef.name, ptype));
    }
    
    FunctionInfo* finfo = func_info_new(span, func.name, func.linkage_name, func.name, func.is_variadic, params, func.return_type);
    context_add_function(sema->current_context, func.name, (void*)finfo);
    register_sym(sema, func.name, syminfo_new(func.name, func.name_span, func.return_type, sym_function_k));
}

void enter_new_context(Semantics* sema) {
    Context* new_ctx = create_new_context(sema->current_context);
    sema->current_context = new_ctx;
}

void leave_context(Semantics* sema) {
    Context* prev_ctx = sema->current_context;
    if (get_context_parent(prev_ctx) != NULL) {
        sema->current_context = get_context_parent(prev_ctx);
    }
    else sema->current_context = sema->root;
}

void add_type(Semantics* sema, const char* name, TypeInfo* type) {
    context_add_type(sema->current_context, name, type);
}

void register_sym(Semantics* sema, const char* name, SymInfo* sym) {
    context_add_sym(sema->current_context, name, (void*)sym);
}

SymInfo* get_syminfo(Semantics* sema, const char* identifer) {
    return context_get_sym(sema->current_context, identifer);
}

FunctionInfo* get_funcinfo(Semantics* sema, const char* name)  {
    return context_get_function(sema->current_context, name);
}

CJVec* sema_convert_param(Semantics* sema, Params params) {
    CJVec* vec = cjvec_new(global_arena);
    size_t max = params.count;
    for (size_t i = 0; i < max; ++i) {
        ParamDef paramdef = params.items[i];
        const char*   type     = type_get_repr(check_type(sema, paramdef.type));
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
    
    const char* functype = type_get_repr(final_ty);    
    CFunction* func = NULL;

    CJVec* params = sema_convert_param(sema, funcdef.params);
    
    if (funcdef.is_decl) {
        func =  cctx_create_function(sema->cctx, funcdef.name, params, functype, false);        
    }
    else {
        func =  cctx_create_function(sema->cctx, funcdef.name, params, functype, true);
        enter_new_context(sema);
        for (size_t i = 0; i < funcdef.params.count; ++i) {
            ParamDef pdef = funcdef.params.items[i];
            register_sym(sema, pdef.name, syminfo_new(pdef.name, pdef.span, check_type(sema, pdef.type), sym_variable_k));
        }
        
        ExprResult body_expr = check_expr(sema, funcdef.body);
        if (funcdef.body->kind != expr_compound_stmt_k) {
            cctx_terminate_expr(sema->cctx, body_expr.expr);
        }
        
        leave_context(sema);
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

        const char* var_type = type_get_repr(final_ty);
        cctx_assign_value(sema->cctx, var_type, vardecl.identifer, expr.expr);
        register_sym(sema, vardecl.identifer, syminfo_new(vardecl.identifer, vardecl.span, final_ty, sym_variable_k));
    } else if (stmt->kind == stmt_expr_k) {
        Expr* expr = stmt->data.expr;
        ExprResult expr_res = check_expr(sema, expr);
        cctx_terminate_expr(sema->cctx, expr_res.expr);
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
        CExpr* cexpr = cctx_create_value_int(sema->cctx,
        type_get_repr(get_type_info(sema, "int")),
        expr->data.literal.lit.int_value);
        
        return eresult_new(cexpr, get_type_info(sema, "int"));
    } break;
    case expr_cstring_k: {
        CExpr* cexpr = cctx_create_value_string(sema->cctx,
        type_get_repr(get_type_info(sema, "cstr")),
        expr->data.literal.lit.cstring_value);
        
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
    case expr_funccall_k: {
        FuncCallExpr call   = expr->data.call_expr;
        FunctionInfo* finfo = NULL;
        const char* func_name = NULL;
        
        switch (call.callee->kind) {
        case expr_ident_k: {
            ExprResult ident_res = check_expr(sema, call.callee);
            if (!ident_res.expr) {
                return eresult_new(NULL, get_type_info(sema, "int"));
            }
            func_name = call.callee->data.literal.lit.str_value;
            SymInfo* info = get_syminfo(sema, func_name);
            if (sym_type(info) != sym_function_k) {
                CJBuffer* buf = cjb_create(global_arena);
                cjb_appendf(buf, "symbol '%s' cannot be called as a function", func_name);
                add_diagnostic(sema, make_generic_error(call.callee->span, cjb_str(buf), NULL));
                return eresult_new(NULL, get_type_info(sema, "int"));
            }

            finfo = get_funcinfo(sema, func_name);
        } break;
        default:
            TODO("invalid callee");
        }
        
        size_t arity_got = cjvec_len(call.args);
        size_t arity_ex  = func_info_get_arity(finfo);
        if (arity_got != arity_ex && !is_variadic_function(finfo)) {
            CJBuffer* tmp  =  cjb_create(global_arena);
            cjb_appendf(tmp, "invalid argument count. expected %zu but got %zu instead", arity_ex, arity_got);
            add_diagnostic(sema, make_generic_error(expr->span, cjb_str(tmp), "please confirm arity"));
        }

        CJVec* args = cjvec_new(global_arena);
        FOREACH(Expr*, arg, i, call.args) {
            ExprResult arg_res = check_expr(sema, arg);
            if (i < arity_ex) {
                // these are the parameters we can check
                ParamInfo* pinfo   = get_param_info(finfo, i);
                TypeInfo*  ptype =   pinfo->type;
                if (!type_match(sema, ptype, arg_res.type)) {
                    CJBuffer* tmp  =  cjb_create(global_arena);
                    cjb_appendf(tmp, "cannot pass '%s' to type '%s'", type_get_name(arg_res.type), type_get_name(ptype));
                    add_diagnostic(sema, make_generic_error(arg->span, cjb_str(tmp), NULL));                    
                }
            }            
            cjvec_push(args, (void*)arg_res.expr);
        }
        return eresult_new(cctx_call(sema->cctx, func_name, args), get_function_type(finfo));
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
    if (!context_has_type(sema->current_context, type_name)) {
        return NULL;
    }
    return context_get_type(sema->current_context, type_name);
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
        if (type_get_kind(type) == type_cstring_k) {
            return get_type_info(sema, "cstr");
        }
        if (type_get_kind(type) == type_none_k) {
            return get_type_info(sema, "none");
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
            add_func(sema, item->data.fndef, item->span);
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
