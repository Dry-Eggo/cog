#include <semantics.h>
#include <defines.h>
#include <stdlib.h>
#include <nodes.h>
#include <functions.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    const char* result;
    const char* preamble;

    type_t*     type;
} expr_result_t;

typedef struct {
    const char* result;    
} stmt_result_t;

stmt_result_t sresult_new(const char* s) {
    return (stmt_result_t) { s };
}

expr_result_t eresult_new(const char* s, const char* p, type_t* ty) {
    return (expr_result_t) { s, p, ty, };
}


semantics_t* semantics_init(juve_vec_t* items, juve_vec_t* source_lines, const char* source, compile_options_t* opts) {
    semantics_t* sema = alloc(semantics_t);
    sema->program = items;
    sema->source_lines = source_lines;
    sema->source = source;
    sema->options = opts;
    
    sema->diagnostics = jvec_new();
    sema->tmp_out = jb_create();
    
    sema->functions = jmap_new();
    sema->symbols   = jmap_new();
    sema->types = jmap_new();


    // initialize builtin types
    // todo: move to a seperate function
    jmap_put(sema->types, "int", type_new(type_int_k, "int", "int"));
    jmap_put(sema->types, "cstr", type_new(type_int_k, "cstr", "const char*"));
    jmap_put(sema->types, "none", type_new(type_int_k, "none", "void"));
    
    return sema;
}

void check_function(semantics_t* sema, item_t* item);
expr_result_t check_expr(semantics_t* sema, expr_t* expr);
stmt_result_t check_stmt(semantics_t* sema, stmt_t* stmt);

// returns stored types out of parsed ones
type_t* check_type(semantics_t* sema, type_t* type);
type_t* get_type_info(semantics_t* sema, const char* type_name);
bool type_match(semantics_t* sema, type_t* t1, type_t* t2);

void stream_out(semantics_t* sema, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char str[1024] = {0};
    vsprintf(str, fmt, args);
    
    jb_appendf_a(sema->tmp_out, global_arena, str);
    va_end(args);
}

void add_func(semantics_t* sema, funcdef_t func) {
    function_info_t* finfo = func_info_new(func.name, func.linkage_name, func.name, NULL, func.return_type);
    jmap_put(sema->functions, func.name, (void*)finfo);
}

void add_type(semantics_t* sema, const char* name, type_t* type) {
    jmap_put(sema->types, name, (void*)type);
}

void check_function(semantics_t* sema, item_t* item) {
    funcdef_t funcdef = item->data.fndef;
    if (funcdef.is_extern) stream_out(sema, "extern ");
    stream_out(sema, "int %s ()\n", funcdef.name);
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
}

stmt_result_t check_stmt(semantics_t* sema, stmt_t* stmt) {
    juve_buffer_t* code = jb_create();
    if (stmt->kind == stmt_vardecl_k) {
        vardecl_t vardecl = stmt->data.vardecl;
        expr_result_t expr = check_expr(sema, vardecl.rhs);
        jb_appendf_a(code, global_arena, "\n\tint %s = %s;", vardecl.identifer, expr.result);
    }
    return sresult_new(jb_str_a(code, global_arena));
}

expr_result_t check_expr(semantics_t* sema, expr_t* expr) {
    juve_buffer_t* code = jb_create();
    switch(expr->kind) {
    case expr_compound_stmt_k: {
        block_t block = expr->data.block;
        int stmt_count = jvec_len(block.statements);
        for (int i = 0; i < stmt_count; ++i) {
            stmt_t* stmt = (stmt_t*)jvec_at(block.statements, i);
            stmt_result_t res = check_stmt(sema, stmt);
            jb_appendf_a(code, global_arena, "%s", res.result);
        }
        
        return eresult_new(jb_str_a(code, global_arena), NULL, NULL);
    } break;
    case expr_int_k: {
        jb_appendf_a(code, global_arena, "%ld", expr->data.literal.lit.int_value);
        return eresult_new(jb_str_a(code, global_arena), NULL, get_type_info(sema, "int"));
    } break;    
    default:
        todo("semantics_t::check_expr::default");
        break;
    }
    return eresult_new(NULL, NULL, NULL);
}

type_t* get_type_info(semantics_t* sema, const char* type_name) {
    if (!jmap_has(sema->types, type_name)) {
        return NULL;
    }
    return jmap_get(sema->types, type_name);
}

bool type_match(semantics_t* sema, type_t* t1, type_t* t2) {
    // todo: more structural comparison
    return t1->kind == t2->kind;
}

type_t*  check_type(semantics_t* sema, type_t* type) {

    if (type) {
        if (type->kind == type_int_k) {
            type_t* int_type = get_type_info(sema, "int");
            return int_type;
        }
    }
    todo("semantics_t::check_type: add more types");
    return NULL;
}

bool sema_run_first_pass(semantics_t* sema) {
    int item_count = jvec_len(sema->program);
    for (int i = 0; i < item_count; ++i) {
        item_t* item = (item_t*)jvec_at(sema->program, i);
        if (item->kind == item_function_k) {
            add_func(sema, item->data.fndef);
        }
    }
    return true;
}

bool sema_run_second_pass(semantics_t* sema) {
    int item_count = jvec_len(sema->program);
    for (int i = 0; i < item_count; ++i) {
        item_t* item = (item_t*)jvec_at(sema->program, i);
        if (item->kind == item_function_k) {
            check_function(sema, item);
        }
    }
    return true;
}

bool sema_check(semantics_t* sema) {        
    if (!sema_run_first_pass(sema)) return false;
    if (!sema_run_second_pass(sema)) return false;
    return true;
}
