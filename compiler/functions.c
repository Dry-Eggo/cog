#include <functions.h>
#include <defines.h>

struct function_info_s {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;
    bool        variadic;
    CJVec*      params;
    TypeInfo*   return_type;
    Span        declaration_span;
};

ParamInfo* param_info_new(const char* name, TypeInfo* type) {
    ParamInfo* pinfo = ALLOC(ParamInfo);
    pinfo->name = name;
    pinfo->type = type;
    return pinfo;
}

size_t func_info_get_arity(FunctionInfo* finfo) {
    if (!finfo) return -1;
    return cjvec_len(finfo->params);
}

bool is_variadic_function(FunctionInfo* finfo) {
    if (!finfo) return false;
    return finfo->variadic;
}

ParamInfo* get_param_info(FunctionInfo* finfo, size_t i) {
    return cjvec_at(finfo->params, i);
}

FunctionInfo* func_info_new(Span span, const char* name, const char* linkage_name, const char* mangled_name, bool variadic, CJVec* params, TypeInfo* type) {
    FunctionInfo* finfo = ALLOC(FunctionInfo);
    finfo->name = name;
    finfo->linkage_name = linkage_name;
    finfo->mangled_name = mangled_name;
    finfo->declaration_span = span;
    finfo->params = params;
    finfo->variadic = variadic;
    finfo->return_type = type;
    return finfo;
}
