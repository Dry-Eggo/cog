#include <parser.h>
#include <nodes.h>
#include <compiler.h>

parser_t* parser_new(juve_vec_t* tokens, const char* source) {
    parser_t* parser = (parser_t*)jarena_alloc(global_arena, sizeof(parser_t));
    parser->tokens = tokens;
    parser->source = source;
    parser->items = jvec_new();
    parser->cursor = 0;
    return parser;
}

token_t* parser_now(parser_t* parser);
token_t* parser_peek(parser_t* parser);
token_t* parser_before(parser_t* parser);
item_t*  parse_item(parser_t* parser);

bool match(parser_t* parser, token_kind_t k);
void recover(parser_t* parser);
void recover_until(parser_t* parser, token_kind_t k);

token_t* parser_now(parser_t* parser) {
    if (parser->cursor < jvec_len(parser->tokens)) {
	return (token_t*)jvec_at(parser->tokens, parser->cursor);
    }
    // guaranteed to be eof_token
    return (token_t*)jvec_back(parser->tokens);
}

token_t* parser_peek(parser_t* parser) {
    if (parser->cursor + 1 < jvec_len(parser->tokens)) {
	return (token_t*)jvec_at(parser->tokens, parser->cursor + 1);
    }
    // guaranteed to be eof_token
    return (token_t*)jvec_back(parser->tokens);
}

token_t* parser_before(parser_t* parser) {
    if (parser->cursor - 1 > 0) {
	return (token_t*)jvec_at(parser->tokens, parser->cursor - 1);
    }
    // guaranteed to be eof_token
    return (token_t*)jvec_back(parser->tokens);
}

item_t* parse_item(parser_t* parser) {
    item_t* item = NULL;
    if (match(parser, token_func_k)) {

    } else {
	
    }
    return item;
}

bool parser_parse(parser_t* parser) {
    bool ok = true;

    while (parser_now(parser)->kind != token_eof_k) {
	item_t* item = parse_item(parser);
	jvec_push(parser->items, (void*)item);
    }
    
    return ok;
}

void parser_free(parser_t* parser) {
    if (parser) {
	if (parser->items) jvec_free(parser->items);
    }
}
