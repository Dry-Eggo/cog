#include <nodes.h>
#include <compiler.h>
#include <defines.h>

Expr* expr_make_block(CJVec* stmts) {
    Expr* expr = alloc(Expr);
    expr->kind = expr_compound_stmt_k;
    expr->data.block.statements = stmts;
    return expr;
}

Expr* expr_make_literal_int(long value, Span span) {
    Expr* expr = alloc(Expr);
    expr->kind = expr_int_k;
    expr->data.literal.lit.int_value = value;
    expr->span = span;
    return expr;    
}

Expr* expr_make_identifier(const char* name, Span span) {
    Expr* expr = alloc(Expr);
    expr->kind = expr_ident_k;
    expr->data.literal.lit.str_value = name;
    expr->span = span;
    return expr;        
}
