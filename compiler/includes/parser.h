#pragma once
#include <enum.h>
#include <juve_utils.h>
#include <compiler_opt.h>
#include <compiler.h>

typedef enum {
    parse_func_body_k,
    parse_vardecl_expr_k,
    
    parse_context_no_set
} ParseContext;

typedef struct {
    CJVec* tokens; // from lexer
    const char* source; // from lexer

    CJVec* items;  // all top-level items
    CJVec* errors;

    ParseContext current_context;

    size_t cursor;
} Parser;

Parser* parser_new(CompileOptions* opts, CJVec* tokens, const char* source);
bool parser_parse(Parser* parser);
void parser_free(Parser* parser);
