#pragma once
#include <compiler.h>
#include <compiler_opt.h>

typedef struct semantics_s semantics_t;

semantics_t* semantics_init(cjvec_t* items, juve_vec_t* source_lines, const char* source, compile_options_t* opts);
cjvec_t* sema_get_diagnostics(semantics_t* sema);
void sema_free(semantics_t* sema);
bool sema_check(semantics_t*);
juve_buffer_t* sema_get_tmp(semantics_t* sema);
