#pragma once
#include <enum.h>
#include <juve_utils.h>
#include <compiler_opt.h>

typedef enum {
    parse_func_body_k,
    parse_vardecl_expr_k,
    
    parse_context_no_set
} parse_context_t;

typedef struct {
    juve_vec_t* tokens; // from lexer
    const char* source; // from lexer

    juve_vec_t* items;  // all top-level items
    juve_vec_t* errors;

    parse_context_t current_context;

    size_t cursor;
} parser_t;

parser_t* parser_new(compile_options_t* opts, juve_vec_t* tokens, const char* source);
bool parser_parse(parser_t* parser);
void parser_free(parser_t* parser);
