#include <functions.h>
#include <defines.h>


struct function_info_s {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;

    juve_vec_t* param_info;
    TypeInfo*     return_type;
};

ParamInfo* param_info_new(const char* name, TypeInfo* type) {
    ParamInfo* pinfo = alloc(ParamInfo);
    todo("param_info: new");
    return pinfo;
}

FunctionInfo* func_info_new(const char* name, const char* linkage_name, const char* mangled_name, juve_vec_t* param_info, TypeInfo* type) {
    FunctionInfo* finfo = alloc(FunctionInfo);
    finfo->name = name;
    finfo->linkage_name = linkage_name;
    finfo->mangled_name = mangled_name;
    finfo->param_info = param_info;
    finfo->return_type = type;
    return finfo;
}
