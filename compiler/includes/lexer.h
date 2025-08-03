#pragma once

#include <token.h>
#include <juve/juve_utils.h>
#include <compiler_opt.h>
#include <compiler.h>

typedef struct {
    size_t cursor;
    size_t line;
    size_t column;
    size_t source_size;
    
    const char* source;
    const char* source_path;
    
    JVec tokens;
} Lexer;

Lexer* lexer_new(CompileOptions*, const char* source);
bool lexer_lex(Lexer* lexer);
void lexer_free(Lexer*);
