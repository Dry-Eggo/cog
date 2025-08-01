#pragma once

#include <compiler.h>
#include <compiler_opt.h>
#include <c_backend.h>
#include <color.h>
#include <lexer.h>
#include <parser.h>
#include <semantics.h>

typedef enum {
    phase_lexer_k,
    phase_parser_k,
    phase_sema_k,
    phase_codegen_k
} CompilerPhase;


typedef struct {
    CompileOptions* options;
    CompilerPhase    phase;

    JVec*        source_lines;
    JBuffer*    source_buffer;
    const char*        source;
    
    Lexer*           lexer;
    Parser*          parser;
    Semantics*       sema;
} Driver;


Driver* driver_new(const char* source, CompileOptions* opts);

void cog_compile(CompileOptions* opts);
