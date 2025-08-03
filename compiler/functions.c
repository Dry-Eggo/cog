#include <functions.h>
#include <defines.h>

struct function_info_s {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;
    bool        variadic;
    JVec        params;
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
    return jvec_len(finfo->params);
}

bool is_variadic_function(FunctionInfo* finfo) {
    if (!finfo) return false;
    return finfo->variadic;
}

ParamInfo* get_param_info(FunctionInfo* finfo, size_t i) {
    return jvec_at(&finfo->params, i);
}

TypeInfo* get_function_type(FunctionInfo* finfo) { return finfo->return_type; }
const char* func_info_get_mname(FunctionInfo* finfo) { return finfo->mangled_name; }

FunctionInfo* func_info_new(Span span, const char* name, const char* linkage_name, const char* mangled_name, bool variadic, JVec params, TypeInfo* type) {
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
