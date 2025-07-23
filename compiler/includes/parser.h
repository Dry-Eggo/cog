#pragma once
#include <enum.h>
#include <juve_utils.h>

typedef struct {
    juve_vec_t* tokens; // from lexer
    const char* source; // from lexer

    juve_vec_t* items;  // all top-level items

    size_t cursor;
} parser_t;

parser_t* parser_new(juve_vec_t* tokens, const char* source);
bool parser_parse(parser_t* parser);
void parser_free(parser_t* parser);
