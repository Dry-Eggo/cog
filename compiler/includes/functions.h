#pragma once
#include <compiler.h>
#include <types.h>
#include <token.h>


typedef struct {
    const char* name;
    TypeInfo*     type;
} ParamInfo;

typedef struct function_info_s FunctionInfo;

ParamInfo*    param_info_new(const char* name, TypeInfo* type);
FunctionInfo* func_info_new(Span span, const char* name, const char* linkage_name, const char* mangled_name, bool variadic, CJVec* param_info, TypeInfo* type);
size_t        func_info_get_arity(FunctionInfo* finfo);
bool          is_variadic_function(FunctionInfo* finfo);
ParamInfo*    get_param_info(FunctionInfo* finfo, size_t i);
