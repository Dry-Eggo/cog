#include <nodes.h>
#include <compiler.h>
#include <defines.h>

item_t* item_make_fndef(funcdef_t fn, span_t span) {
    item_t* item = alloc(item_t);

    item->span = span;
    item->data.fndef = fn;
    item->kind = item_function_k;
    
    return item;
}


stmt_t* stmt_make_vardecl(vardecl_t vardecl, span_t span) {
    vardecl.span = span;
    stmt_t* stmt = alloc(stmt_t);
    stmt->kind = stmt_vardecl_k;     
    stmt->data.vardecl = vardecl;    
    stmt->span = span;
    return stmt;
}
