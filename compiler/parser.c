#include <parser.h>
#include <nodes.h>
#include <compiler.h>
#include <errors.h>
#include <stdio.h>
#include <types.h>
#include <defines.h>

#define err_semi(p) add_error(p, make_syntax_error(*parser_before(p)->span, "Unqualified token", "did you forget a ';'"))
#define get_span(p) (*parser_now(p)->span)

parser_t* parser_new(compile_options_t* opts, juve_vec_t* tokens, const char* source) {
    parser_t* parser = (parser_t*)jarena_alloc(global_arena, sizeof(parser_t));
    parser->tokens = tokens;
    parser->source = source;
    parser->items = jvec_new();
    parser->errors = jvec_new();
    parser->cursor = 0;
    parser->current_context = parse_context_no_set;
    return parser;
}

token_t* parser_now(parser_t* parser);
token_t* parser_peek(parser_t* parser);
token_t* parser_before(parser_t* parser);
item_t*  parse_item(parser_t* parser);
item_t*  parse_function(parser_t* parser);

expr_t*  parse_body(parser_t* parser);
expr_t*  parse_expr(parser_t* parser);
expr_t*  parse_logical_or(parser_t* parser);
expr_t*  parse_logical_and(parser_t* parser);
expr_t*  parse_equality(parser_t* parser);
expr_t*  parse_conditional(parser_t* parser);
expr_t*  parse_additive(parser_t* parser);
expr_t*  parse_term(parser_t* parser);
expr_t*  parse_atom(parser_t* parser);

stmt_t*  parse_stmt(parser_t* parser);

type_t* parse_type(parser_t* parser);

void     parser_advance(parser_t* parser);
bool match(parser_t* parser, token_kind_t k);
void recover(parser_t* parser);
void recover_until(parser_t* parser, token_kind_t k);
bool expect(parser_t* parser, token_kind_t k);
void add_error(parser_t* parser, syntax_error_t* error);

void recover(parser_t* parser) {
    while (!match(parser, token_eof_k) && !match(parser, token_semi_k)) {
	    parser_advance(parser);
    }
}

void add_error(parser_t* parser, syntax_error_t* error) {
    jvec_push(parser->errors, (void*)error);
}

bool expect(parser_t* parser, token_kind_t k) {
    if (match(parser, k)) {
	    parser_advance(parser);
	    return true;
    }
    return false;
}

void recover_until(parser_t* parser, token_kind_t k) {
    while (!match(parser, token_eof_k) && !match(parser, k)) {
	    parser_advance(parser);
    }
}

void parser_advance(parser_t* parser) {
    parser->cursor++;
}

bool match(parser_t* parser, token_kind_t k) {
    return (parser_now(parser)->kind == k);
}

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
    if (parser->cursor > 0) {
	    return (token_t*)jvec_at(parser->tokens, parser->cursor - 1);
    }
    // guaranteed to be eof_token
    return (token_t*)jvec_back(parser->tokens);
}

item_t* parse_function(parser_t* parser) {
    item_t* item = (item_t*)jarena_alloc(global_arena, sizeof(item_t));
    type_t* return_type = type_new(type_none_k, "none", "void");
    expect(parser, token_func_k);
    funcdef_t funcdef = {0};
    
    if (match(parser, token_ident_k)) {
	    funcdef.name = parser_now(parser)->text;
	    parser_advance(parser);
    } else {
	    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Expected name after 'func'", NULL));
	    recover_until(parser, token_oparen_k);
    }

    if (match(parser, token_oparen_k)) {
	    parser_advance(parser);
	    if (!expect(parser, token_cparen_k))  {
	        recover_until(parser, token_eof_k);
	    }
    }

    if (match(parser, token_slim_arrow_k)) {
        parser_advance(parser);
        return_type = parse_type(parser);
        if (!return_type) {
            recover_until(parser, token_eof_k);
        }        
    }

    funcdef.return_type = return_type;
    
    parser->current_context = parse_func_body_k;
    funcdef.body = parse_expr(parser);
    parser->current_context = parse_context_no_set;
    
    item->kind = item_function_k;
    item->data.fndef = funcdef;
    return item;
}

stmt_t* parse_stmt(parser_t* parser) {
    stmt_t* stmt = NULL;
    if (match(parser, token_let_k)) {
        parser_advance(parser);

        vardecl_t vardecl = {0};
        
        bool is_const = true;        
        const bool is_uninit = false; // always false for 'let' bindings        
        
        if (match(parser, token_ident_k)) {
            vardecl.identifer = parser_now(parser)->text;
            parser_advance(parser);
        } else if (match(parser, token_oparen_k)) {
            todo("parser_t::parse_stmt::parse_vardecl: parse_pattern");
        } else {
            add_error(parser, make_syntax_error(*parser_now(parser)->span, "Unqualified token", "expected an identifier or pattern"));
            recover_until(parser, token_semi_k);
            return NULL;
        }

        if (match(parser, token_coleq_k)) {
            vardecl.type = type_new(type_cstring_k, "cstr", NULL);
            parser_advance(parser);
        } else {
            todo("parser_t::parse_stmt::parse_vardecl: parse vardecl type");
        }

        parse_context_t prev_ctx = parser->current_context;
        parser->current_context = parse_vardecl_expr_k;
        vardecl.rhs = parse_expr(parser);
        parser->current_context = prev_ctx;
        
        if (!expect(parser, token_semi_k)) {
            err_semi(parser);
        }
        stmt = alloc(stmt_t);
        stmt->kind = stmt_vardecl_k;        
        stmt->data.vardecl = vardecl;
        return stmt;
    }
    todo("parser_t::parse_stmt: '%s'", parser_now(parser)->text);
    return stmt;
}

item_t* parse_item(parser_t* parser) {
    item_t* item = NULL;
    if (match(parser, token_func_k)) {
	    item = parse_function(parser);
    } else {
	    todo("parser_t::parse_item: other items");
    }
    return item;
}

expr_t*  parse_expr(parser_t* parser) {
    expr_t* expr = parse_logical_or(parser);
    return expr;
}

expr_t*  parse_logical_or(parser_t* parser) {
    expr_t* lhs = parse_logical_and(parser);
    return lhs;
}

expr_t*  parse_logical_and(parser_t* parser) {
    expr_t* lhs = parse_equality(parser);
    return lhs;
}

expr_t*  parse_equality(parser_t* parser) {
    expr_t* lhs = parse_conditional(parser);
    return lhs;
}

expr_t*  parse_conditional(parser_t* parser) {
    expr_t* lhs = parse_additive(parser);
    return lhs;
}

expr_t*  parse_additive(parser_t* parser) {
    expr_t* lhs = parse_term(parser);
    return lhs;
}

expr_t*  parse_term(parser_t* parser) {
    expr_t* lhs = parse_atom(parser);
    return lhs;
}

expr_t*  parse_atom(parser_t* parser) {
    expr_t* expr = NULL;
    if (match(parser, token_obrace_k)) {
	    parser_advance(parser);
	    expr = parse_body(parser);
	    if (!expect(parser, token_cbrace_k)) {
	        recover_until(parser, token_semi_k);
	        return NULL;
	    }
    } else if (match(parser, token_number_k)) {
        span_t span = get_span(parser);
        long value = strtol(parser_now(parser)->text, NULL, 0);
        parser_advance(parser);
        return expr_make_literal_int(value, span);
    } else {
	    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Not a valid expression", NULL));	
	    if (parser->current_context == parse_func_body_k) recover_until(parser, token_eof_k);
        else if (parser->current_context == parse_vardecl_expr_k) recover_until(parser, token_semi_k);
	    return NULL;
    }
    return expr;
}

expr_t* parse_body(parser_t* parser) {
    juve_vec_t* stmts = jvec_new();
    while (!match(parser, token_eof_k) && !match(parser, token_cbrace_k)) {
	    stmt_t* stmt = parse_stmt(parser);
        jvec_push(stmts, (void*)stmt);
    }    
    return expr_make_block(stmts);
}

type_t* parse_type(parser_t* parser) {
    // type_t::repr should not be set by parser as the target backend
    // decides what type_t::repr becomes
    if (match(parser, token_int_k)) {
        parser_advance(parser);
        return type_new(type_int_k, "int", NULL);
    } else if (match(parser, token_cstr_k)) {
        parser_advance(parser);
        return type_new(type_int_k, "cstr", NULL);
    } else if (match(parser, token_none_k)) {
        parser_advance(parser);
        return type_new(type_int_k, "int", NULL);
    } else {
        todo("parser::parse_type: custom typenames");
    }
    
    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Invalid type", NULL));   
    return NULL;
}

bool parser_parse(parser_t* parser) {
    while (!match(parser, token_eof_k)) {
	    item_t* item = parse_item(parser);
	    jvec_push(parser->items, (void*)item);
    }
    
    return jvec_len(parser->errors) == 0;
}

void parser_free(parser_t* parser) {
    if (parser) {
	    if (parser->items) jvec_free(parser->items);
    }
}
