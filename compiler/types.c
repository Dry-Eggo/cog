#include <types.h>
#include <compiler.h>
#include <defines.h>

struct type_s {
    const char* name;
    const char* repr;
    type_kind_t kind;    
};


type_t* type_new(type_kind_t kind, const char* name, const char* repr) {
    type_t* type = alloc(type_t);
    type->kind = kind;
    type->name = name;
    type->repr = repr;
    return type;
}

type_kind_t type_get_kind(type_t* t) { return t->kind; }
const char* type_get_name(type_t* t) { return t->name; }
const char* type_get_repr(type_t* t) { return t->repr; }
