#include <token.h>
#include <compiler.h>

span_t* span_new(size_t line, size_t col, size_t off, const char* filename) {
    span_t* span = (span_t*)jarena_alloc(global_arena, sizeof(span_t));
    span->line = line;
    span->column = col;
    span->offset = off;
    span->filename = filename;
    return span;
}

span_t span_merge(span_t* start, span_t* end) {
    start->offset = end->offset;
    return *start;
}

token_t* token_new(span_t* span, token_kind_t kind, const char* text) {
    token_t* tok = (token_t*)jarena_alloc(global_arena, sizeof(token_t));
    tok->span = span;
    tok->kind = kind;
    tok->text = text;
    return tok;
}
