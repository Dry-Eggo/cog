#include <functions.h>
#include <defines.h>


struct function_info_s {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;

    JVec* param_info;
    TypeInfo*     return_type;
};

ParamInfo* param_info_new(const char* name, TypeInfo* type) {
    ParamInfo* pinfo = ALLOC(ParamInfo);
    pinfo->name = name;
    pinfo->type = type;
    return pinfo;
}

FunctionInfo* func_info_new(const char* name, const char* linkage_name, const char* mangled_name, JVec* param_info, TypeInfo* type) {
    FunctionInfo* finfo = ALLOC(FunctionInfo);
    finfo->name = name;
    finfo->linkage_name = linkage_name;
    finfo->mangled_name = mangled_name;
    finfo->param_info = param_info;
    finfo->return_type = type;
    return finfo;
}
