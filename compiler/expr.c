#include <nodes.h>
#include <compiler.h>
#include <defines.h>

expr_t* expr_make_block(juve_vec_t* stmts) {
    expr_t* expr = alloc(expr_t);
    expr->kind = expr_compound_stmt_k;
    expr->data.block.statements = stmts;
    return expr;
}

expr_t* expr_make_literal_int(long value, span_t span) {
    expr_t* expr = alloc(expr_t);
    expr->kind = expr_int_k;
    expr->data.literal.lit.int_value = value;
    expr->span = span;
    return expr;    
}
