#pragma once
#include <compiler.h>
#include <compiler_opt.h>

typedef struct {
    juve_vec_t* program;
    juve_vec_t* diagnostics;
    juve_vec_t* source_lines;

    juve_map_t* functions;
    juve_map_t* types;
    juve_map_t* symbols;
    
    compile_options_t* options;

    const char* source;

    // for temporar visualizaton of the generated code
    // will be delegated to a proper backend dispatcher
    juve_buffer_t* tmp_out;
} semantics_t;

semantics_t* semantics_init(juve_vec_t* items, juve_vec_t* source_lines, const char* source, compile_options_t* opts);
bool sema_check(semantics_t*);
