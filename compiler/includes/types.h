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
} type_kind_t;

typedef struct {
    const char* name;
    const char* repr;
    type_kind_t kind;    
} type_t;


type_t* type_new(type_kind_t kind, const char* name, const char* repr);
