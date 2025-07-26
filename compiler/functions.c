#include <functions.h>
#include <defines.h>


struct function_info_s {
    const char* name;
    const char* linkage_name;
    const char* mangled_name;

    juve_vec_t* param_info;
    type_t*     return_type;
};

param_info_t* param_info_new(const char* name, type_t* type) {
    param_info_t* pinfo = alloc(param_info_t);
    todo("param_info: new");
    return pinfo;
}

function_info_t* func_info_new(const char* name, const char* linkage_name, const char* mangled_name, juve_vec_t* param_info, type_t* type) {
    function_info_t* finfo = alloc(function_info_t);
    finfo->name = name;
    finfo->linkage_name = linkage_name;
    finfo->mangled_name = mangled_name;
    finfo->param_info = param_info;
    finfo->return_type = type;
    return finfo;
}
