#pragma once
#include <compiler.h>
#include <compiler_opt.h>

typedef struct c_context_s c_context_t;

c_context_t* c_ctx_new(cjvec_t* program, compile_options_t* opts);
bool c_ctx_emit(c_context_t* ctx);
void c_ctx_free(c_context_t* ctx);
