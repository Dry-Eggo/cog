#include <variables.h>
#include <compiler.h>
#include <defines.h>

struct symbol_info_s {
    const char* name;
    Span declaration;
    TypeInfo* type;

    SymType kind;
};


SymInfo* syminfo_new(const char* name, Span decl, TypeInfo* type, SymType kind) {
    SymInfo* sym = ALLOC(SymInfo);
    sym->name = name;
    sym->declaration = decl;
    sym->type = type;
    sym->kind = kind;
    return sym;
}

TypeInfo* syminfo_get_type(SymInfo* sym) {
    if (!sym) return NULL;
    return sym->type;
}
