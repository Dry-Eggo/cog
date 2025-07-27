#include <token.h>
#include <compiler.h>

Span* span_new(size_t line, size_t col, size_t off, const char* filename) {
    Span* span = (Span*)jarena_alloc(global_arena, sizeof(Span));
    span->line = line;
    span->column = col;
    span->offset = off;
    span->filename = filename;
    return span;
}

Span span_merge(Span* start, Span* end) {
    start->offset = end->offset;
    return *start;
}

Token* token_new(Span* span, token_kind_t kind, const char* text) {
    Token* tok = (Token*)jarena_alloc(global_arena, sizeof(Token));
    tok->span = span;
    tok->kind = kind;
    tok->text = text;
    return tok;
}
