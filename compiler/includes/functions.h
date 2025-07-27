#pragma once
#include <compiler.h>
#include <types.h>


typedef struct {
    const char* name;
    TypeInfo*     type;
} ParamInfo;

typedef struct function_info_s FunctionInfo;

ParamInfo* param_info_new(const char* name, TypeInfo* type);
FunctionInfo* func_info_new(const char* name, const char* linkage_name, const char* mangled_name, JVec* param_info, TypeInfo* type);
