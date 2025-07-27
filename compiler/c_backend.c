#include <c_backend.h>
#include <defines.h>
#include <nodes.h>
#include <stdarg.h>

struct c_context_s {
    compile_options_t* options;
    juve_buffer_t*     output;
    cjvec_t*           program;
};


c_context_t* c_ctx_new(cjvec_t* program, compile_options_t* opts) {
    c_context_t* ctx = alloc(c_context_t);
    ctx->program = program;
    
    ctx->output = jb_create();
    
    return ctx;
}

void c_ctx_free(c_context_t* ctx);

void append_out(c_context_t* ctx, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    char str[1024] = {0};
    vsprintf(str, fmt, args);
    
    jb_appendf_a(ctx->output, global_arena, str);
    va_end(args);
}

void emit_stmt(c_context_t* ctx, stmt_t* stmt);
void emit_vardecl(c_context_t* ctx, vardecl_t vardecl);
void emit_expr(c_context_t* ctx, expr_t* expr);

void emit_body(c_context_t* ctx, block_t block) {
    fori(stmt_t*, stmt, i, block.statements) {
        emit_stmt(ctx, stmt);
    }
}

juve_buffer_t* c_ctx_get_output(c_context_t* ctx) { return ctx->output; }

void emit_vardecl(c_context_t* ctx, vardecl_t vardecl) {
    const char* name = vardecl.identifer;
    append_out(ctx, "\n    int %s = ");
    emit_expr(ctx, vardecl.rhs);
    append_out(ctx, ";");
}

void emit_stmt(c_context_t* ctx, stmt_t* stmt) {
    if (stmt->kind == stmt_vardecl_k) {
        emit_vardecl(ctx, stmt->data.vardecl);
    }
}

void emit_expr(c_context_t* ctx, expr_t* expr) {
    if (expr->kind == expr_compound_stmt_k) {
        emit_body(ctx, expr->data.block);
    } else if (expr->kind == expr_int_k) {
        append_out(ctx, "%ld", expr->data.literal.lit.int_value);
    }
    todo("");
}

void emit_function(c_context_t* ctx, funcdef_t fndef) {
    const char* func_name = fndef.name;
    if (fndef.is_extern) append_out(ctx, "extern ");
    append_out(ctx, "%s ()", func_name);

    if (!fndef.is_decl) {
        append_out(ctx, "\n{");
        emit_expr(ctx, fndef.body);
        append_out(ctx, "\n}");
    } else append_out(ctx, ";");
}

void emit_item(c_context_t* ctx, item_t* item) {
    if (item->kind == item_function_k) {
        emit_function(ctx, item->data.fndef);
    }
}

bool c_ctx_emit(c_context_t* ctx) {
    fori(item_t*, item, i, ctx->program) {
        emit_item(ctx, item);
    }
    return true;
}
