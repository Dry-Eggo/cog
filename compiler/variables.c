#include <variables.h>
#include <compiler.h>
#include <defines.h>

struct symbol_info_s {
    const char* name;
    span_t declaration;
    type_t* type;

    symtype_t kind;
};


symbol_info_t* syminfo_new(const char* name, span_t decl, type_t* type, symtype_t kind) {
    symbol_info_t* sym = alloc(symbol_info_t);
    sym->name = name;
    sym->declaration = decl;
    sym->type = type;
    sym->kind = kind;
    return sym;
}

type_t* syminfo_get_type(symbol_info_t* sym) {
    if (!sym) return NULL;
    return sym->type;
}
