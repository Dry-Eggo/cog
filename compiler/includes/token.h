#pragma once

#include <token.h>
#include <stddef.h>
#include <enum.h>
#include <juve_utils.h>

typedef struct {
    size_t line;
    size_t column;
    size_t offset;

    const char* filename;
} span_t;

typedef struct {
    token_kind_t kind;
    const char* text;
    span_t* span;
} token_t;

span_t* span_new(size_t line, size_t col, size_t off, const char* filename);
span_t span_merge(span_t*, span_t*);
token_t* token_new(span_t* span, token_kind_t kind, const char* text);
