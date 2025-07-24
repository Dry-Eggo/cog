#pragma once

#include <token.h>
#include <juve_utils.h>
#include <compiler_opt.h>

typedef struct {
    size_t cursor;
    size_t line;
    size_t column;
    size_t source_size;
    
    const char* source;
    const char* source_path;
    
    juve_vec_t* tokens;
} lexer_t;

lexer_t* lexer_new(compile_options_t*, const char* source);
bool lexer_lex(lexer_t* lexer);
void lexer_free(lexer_t*);
