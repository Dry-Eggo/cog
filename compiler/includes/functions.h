#pragma once
#include <compiler.h>
#include <types.h>


typedef struct {
    const char* name;
    type_t*     type;
} param_info_t;

typedef struct {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;

    juve_vec_t* param_info;
    type_t*     return_type;
} function_info_t;


param_info_t* param_info_new(const char* name, type_t* type);
function_info_t* func_info_new(const char* name, const char* linkage_name, const char* mangled_name, juve_vec_t* param_info, type_t* type);
