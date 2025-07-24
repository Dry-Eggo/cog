#include <nodes.h>
#include <compiler.h>

expr_t* expr_make_block(juve_vec_t* stmts) {
    expr_t* expr = (expr_t*)jarena_alloc(global_arena, sizeof(expr_t));
    expr->kind = expr_compound_stmt_k;
    expr->data.block.statements = stmts;
    return expr;
}
