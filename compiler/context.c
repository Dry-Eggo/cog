#include <context.h>
#include <compiler.h>

struct Context {
    JMap*    symbols;
    JMap*    functions;
    JMap*    types;
    Context* parent;
};

Context* create_new_context(Context* parent) {
    Context* context   = ALLOC(Context);
    context->symbols   = jmap_new();
    context->functions = jmap_new();
    context->types     = jmap_new();
    context->parent    = parent;
    return context;
}

void context_free(Context* context) {
    if (!context) return;    
    jmap_free(context->functions);
    jmap_free(context->types);
    jmap_free(context->symbols);
    // doesn't destroy context->parent
}

Context* get_context_parent(Context* context) {
    if (!context) return NULL;
    return context->parent;
}

bool context_has_sym(Context* context, const char* name) {
    if (!context || !name) return false;
    if (!context->symbols) return false;
    return jmap_has(context->symbols, name);
}

SymInfo* context_get_sym(Context* context, const char* name) {
    if (!context || !name)               return NULL;
    if (!context->symbols)               return NULL;
    if (!context_has_sym(context, name)) return NULL;
    return jmap_get(context->symbols, name);
}

bool context_has_function(Context* context, const char* name) {
    if (!context || !name)   return false;
    if (!context->functions) return false;
    return jmap_has(context->functions, name);    
}

FunctionInfo* context_get_function(Context* context, const char* name) {
    if (!context || !name)                    return NULL;
    if (!context->functions)                  return NULL;
    if (!context_has_function(context, name)) return NULL;
    return jmap_get(context->functions, name);    
}

bool context_has_type(Context* context, const char* name) {
    if (!context || !name)   return false;
    if (!context->types)     return false;
    return jmap_has(context->types, name);    
}

TypeInfo* context_get_type(Context* context, const char* name) {
    if (!context || !name)                    return NULL;
    if (!context->types)                      return NULL;
    if (!context_has_type(context, name))     return NULL;
    return jmap_get(context->types, name);        
}

bool context_add_sym(Context* context, const char* name, SymInfo* sym) {
    if (!context || !name || !sym)       return false;
    if (!context->symbols)               return false;
    if (context_has_sym(context, name))  return true;
    jmap_put(context->symbols, name, (void*)sym);
    return true;
}

bool context_add_function(Context* context, const char* name, FunctionInfo* func) {
    if (!context || !name || !func)                return false;
    if (!context->functions)                       return false;
    if (context_has_function(context, name))       return true;    
    jmap_put(context->functions, name, (void*)func);
    return true;    
}

bool context_add_type(Context* context, const char* name, TypeInfo* type) {
    if (!context || !name || !type)       return false;
    if (!context->types)                  return false;
    if (context_has_type(context, name))  return true;
    jmap_put(context->types, name, (void*)type);
    return true;        
}
