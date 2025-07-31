#pragma once

#include <variables.h>
#include <functions.h>
typedef struct Context Context;

Context*      create_new_context(Context* parent);
void          context_free(Context* context);
Context*      get_context_parent(Context* context);
bool          context_has_sym(Context* context, const char* name);
SymInfo*      context_get_sym(Context* context, const char* name);
bool          context_has_function(Context* context, const char* name);
FunctionInfo* context_get_function(Context* context, const char* name);
bool          context_has_type(Context* context, const char* name);
TypeInfo*     context_get_type(Context* context, const char* name);
bool          context_add_sym(Context* context, const char* name, SymInfo* sym);
bool          context_add_function(Context* context, const char* name, FunctionInfo* sym);
bool          context_add_type(Context* context, const char* name, TypeInfo* sym);
