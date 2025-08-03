#pragma once
#include <enum.h>
#include <juve/juve_utils.h>
#include <compiler_opt.h>
#include <compiler.h>

typedef enum {
    parse_func_body_k,
    parse_vardecl_expr_k,
    
    parse_context_no_set
} ParseContext;

typedef struct {
    JVec tokens; // from lexer
    const char* source; // from lexer

    JVec items;  // all top-level items
    JVec errors;

    ParseContext current_context;

    size_t cursor;
} Parser;

Parser* parser_new(CompileOptions* opts, JVec tokens, const char* source);
bool parser_parse(Parser* parser);
void parser_free(Parser* parser);
