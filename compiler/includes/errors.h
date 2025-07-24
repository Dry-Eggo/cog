#pragma once
#include <token.h>

typedef struct {
    span_t span;
    const char* message;
    const char* hint;
} syntax_error_t;

syntax_error_t* make_syntax_error(span_t span, const char* msg, const char* hint);
void syntax_error_flush(juve_vec_t* errors, juve_vec_t* source);
