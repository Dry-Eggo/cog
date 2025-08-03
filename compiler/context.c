#include <context.h>
#include <compiler.h>

struct Context {
    JMap    symbols;
    JMap    functions;
    JMap    types;
    Context* parent;
};

Context* create_new_context(Context* parent) {
    Context* context   = ALLOC(Context);
    context->symbols   = jmap_create(global_arena);
    context->functions = jmap_create(global_arena);
    context->types     = jmap_create(global_arena);
    context->parent    = parent;
    return context;
}

void context_free(Context* context) {
    UNUSED(context);
    // doesn't destroy context->parent
    // no op
    // structure  lives on the arena
}

Context* get_context_parent(Context* context) {
    if (!context) return NULL;
    return context->parent;
}

bool context_has_sym(Context* context, const char* name) {
    if (!context || !name) return false;
    if (jmap_has(context->symbols, name)) return true;

    if (context->parent) return context_has_sym(context->parent, name);
    return false;
}

SymInfo* context_get_sym(Context* context, const char* name) {
    if (!context || !name)               return NULL;
    if (!jmap_has(context->symbols, name)) {
        if (context->parent) return context_get_sym(context->parent, name);
        return NULL;
    }
    return jmap_get(context->symbols, name);
}

bool context_has_function(Context* context, const char* name) {
    if (!context || !name)   return false;
    if (jmap_has(context->functions, name)) return true;

    if (context->parent) return context_has_function(context->parent, name);
    return false;
}

FunctionInfo* context_get_function(Context* context, const char* name) {
    if (!context || !name)                    return NULL;
    if (!jmap_has(context->functions, name)) {
        if (context->parent) return context_get_function(context->parent, name);
        return NULL;
    }
    return jmap_get(context->functions, name);    
}

bool context_has_type(Context* context, const char* name) {
    if (!context || !name)   return false;
    if (jmap_has(context->types, name)) return true;

    if (context->parent) return context_has_type(context->parent, name);
    return false;
}

TypeInfo* context_get_type(Context* context, const char* name) {
    if (!context || !name)                    return NULL;
    if (!jmap_has(context->types, name)) {
        if (context->parent) return context_get_type(context->parent, name);
        return NULL;
    }
    return jmap_get(context->types, name);        
}

bool context_add_sym(Context* context, const char* name, SymInfo* sym) {
    if (!context || !name || !sym)       return false;
    if (context_has_sym(context, name))  return true;
    jmap_put(&context->symbols, name, (void*)sym);
    return true;
}

bool context_add_function(Context* context, const char* name, FunctionInfo* func) {
    if (!context || !name || !func)                return false;
    if (context_has_function(context, name))       return true;    
    jmap_put(&context->functions, name, (void*)func);
    return true;    
}

bool context_add_type(Context* context, const char* name, TypeInfo* type) {
    if (!context || !name || !type)       return false;
    if (context_has_type(context, name))  return true;
    jmap_put(&context->types, name, (void*)type);
    return true;        
}
