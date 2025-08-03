#pragma once

#include <compiler.h>
#include <token.h>
#include <stddef.h>
#include <enum.h>
#include <juve/juve_utils.h>


typedef struct {
    size_t line;
    size_t column;
    size_t offset;

    const char* filename;
} Span;

typedef struct {
    TokenKind kind;
    const char* text;
    Span* span;
} Token;

Span* span_new(size_t line, size_t col, size_t off, const char* filename);
Span span_merge(Span*, Span*);
Token* token_new(Span* span, TokenKind kind, const char* text);
