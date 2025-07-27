#pragma once

typedef enum {
    type_int_k,
    type_string_k,
    type_cstring_k,
    type_none_k,
    type_any_k, // note: not an 'any' type but a placeholder for inference
    type_bool_k,
    type_nil_k,
    type_count_k,
} TypeKind;

typedef struct type_s TypeInfo;

TypeInfo* type_new(TypeKind kind, const char* name, const char* repr);
TypeKind type_get_kind(TypeInfo* t);
const char* type_get_name(TypeInfo* t);
const char* type_get_repr(TypeInfo* t);
