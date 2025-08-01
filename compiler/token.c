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
    return (Span) {
        .filename = start->filename,
        .line = start->line,
        .column = start->column,
        .offset = end->offset,
    };
}

Token* token_new(Span* span, TokenKind kind, const char* text) {
    Token* tok = (Token*)jarena_alloc(global_arena, sizeof(Token));
    tok->span = span;
    tok->kind = kind;
    tok->text = text;
    return tok;
}
