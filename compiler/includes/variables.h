#pragma once

#include <token.h>
#include <types.h>
#include <defines.h>
#include <stdio.h>

typedef enum {
    sym_variable_k,
    sym_function_k,
} symtype_t;

typedef struct symbol_info_s symbol_info_t;

symbol_info_t* syminfo_new(const char* name, span_t decl, type_t* type, symtype_t kind);
type_t* syminfo_get_type(symbol_info_t* sym);
