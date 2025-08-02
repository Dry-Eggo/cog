#include <nodes.h>
#include <compiler.h>
#include <defines.h>

Expr* expr_make_block(CJVec* stmts) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_compound_stmt_k;
    expr->data.block.statements = stmts;
    return expr;
}

Expr* expr_make_literal_int(long value, Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_int_k;
    expr->data.literal.lit.int_value = value;
    expr->span = span;
    return expr;    
}

Expr* expr_make_binop(BinaryOpExpr binop, Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_binop_k;
    expr->data.binop = binop;
    expr->span = span;
    return expr;
}

Expr* expr_make_no_op(Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_empty_k;
    expr->span = span;
    return expr;    
}

Expr* expr_make_identifier(const char* name, Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_ident_k;
    expr->data.literal.lit.identifier_value = name;
    expr->span = span;
    return expr;        
}

Expr* expr_make_cstring(const char* value, Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_cstring_k;
    expr->data.literal.lit.cstring_value = value;
    expr->span = span;
    return expr;            
}

Expr* expr_make_call(Expr* callee, CJVec* args, Span span) {
    Expr* expr = ALLOC(Expr);
    expr->kind = expr_funccall_k;
    expr->data.call_expr.callee = callee;
    expr->data.call_expr.args = args;
    expr->span = span;
    return expr;                
}
