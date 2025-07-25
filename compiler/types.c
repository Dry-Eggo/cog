#include <types.h>
#include <compiler.h>
#include <defines.h>

type_t* type_new(type_kind_t kind, const char* name, const char* repr) {
    type_t* type = alloc(type_t);
    type->kind = kind;
    type->name = name;
    type->repr = repr;
    return type;
}
