#include <types.h>
#include <compiler.h>
#include <defines.h>

struct type_s {
    const char* name;
    const char* repr;
    TypeKind kind;    
};


TypeInfo* type_new(TypeKind kind, const char* name, const char* repr) {
    TypeInfo* type = ALLOC(TypeInfo);
    type->kind = kind;
    type->name = name;
    type->repr = repr;
    return type;
}

TypeKind type_get_kind(TypeInfo* t) { return t->kind; }
const char* type_get_name(TypeInfo* t) { return t->name; }
const char* type_get_repr(TypeInfo* t) { return t->repr; }
