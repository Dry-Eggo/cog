#pragma once

#include <token.h>
#include <types.h>
#include <defines.h>
#include <stdio.h>

typedef enum {
    sym_variable_k,
    sym_function_k,
} SymType;

typedef struct symbol_info_s SymInfo;

SymInfo* syminfo_new(const char* name, Span decl, TypeInfo* type, SymType kind);
TypeInfo* syminfo_get_type(SymInfo* sym);
