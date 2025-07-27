#include <nodes.h>
#include <compiler.h>
#include <defines.h>

Item* item_make_fndef(FunctionDef fn, Span span) {
    Item* item = alloc(Item);

    item->span = span;
    item->data.fndef = fn;
    item->kind = item_function_k;
    
    return item;
}


Stmt* stmt_make_vardecl(VarDeclStmt vardecl, Span span) {
    vardecl.span = span;
    Stmt* stmt = alloc(Stmt);
    stmt->kind = stmt_vardecl_k;     
    stmt->data.vardecl = vardecl;    
    stmt->span = span;
    return stmt;
}
