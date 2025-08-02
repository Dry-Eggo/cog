#include <parser.h>
#include <nodes.h>
#include <compiler.h>
#include <errors.h>
#include <stdio.h>
#include <types.h>
#include <defines.h>
#include <utils/array.h>

#define err_semi(p) add_error(p, make_syntax_error(*parser_now(p)->span, "Unqualified token", "did you forget a ';'"))
#define get_span(p) (*parser_now(p)->span)
#define get_pspan(p) (parser_now(p)->span)
#define err_param (ParamDef){"error", false, param_span, NULL, NULL}

Parser* parser_new(CompileOptions* opts, CJVec* tokens, const char* source) {
    UNUSED(opts);
    Parser* parser = (Parser*)jarena_alloc(global_arena, sizeof(Parser));
    parser->tokens = tokens;
    parser->source = source;
    parser->items = cjvec_new(global_arena);
    parser->errors = cjvec_new(global_arena);
    parser->cursor = 0;
    parser->current_context = parse_context_no_set;
    return parser;
}

Token* parser_now(Parser* parser);
Token* parser_peek(Parser* parser);
Token* parser_before(Parser* parser);
Item*  parse_item(Parser* parser);
Item*  parse_function(Parser* parser);

Expr*  parse_body(Parser* parser);
Expr*  parse_expr(Parser* parser);
Expr*  parse_logical_or(Parser* parser);
Expr*  parse_logical_and(Parser* parser);
Expr*  parse_equality(Parser* parser);
Expr*  parse_conditional(Parser* parser);
Expr*  parse_additive(Parser* parser);
Expr*  parse_term(Parser* parser);
Expr* parse_postifix(Parser* parser);
Expr*  parse_atom(Parser* parser);

Stmt*  parse_stmt(Parser* parser);

TypeInfo* parse_type(Parser* parser);

void     parser_advance(Parser* parser);
bool match(Parser* parser, TokenKind k);
void recover(Parser* parser);
void recover_until(Parser* parser, TokenKind k);
bool expect(Parser* parser, TokenKind k);
void add_error(Parser* parser, SyntaxError* error);

void recover(Parser* parser) {
    while (!match(parser, token_eof_k) && !match(parser, token_semi_k)) {
	    parser_advance(parser);
    }
}

void add_error(Parser* parser, SyntaxError* error) {
    cjvec_push(parser->errors, (void*)error);
}

bool expect(Parser* parser, TokenKind k) {
    if (match(parser, k)) {
	    parser_advance(parser);
	    return true;
    }
    return false;
}

void recover_until(Parser* parser, TokenKind k) {
    while (!match(parser, token_eof_k) && !match(parser, k)) {
	    parser_advance(parser);
    }
}

void parser_advance(Parser* parser) {
    parser->cursor++;
}

bool match(Parser* parser, TokenKind k) {
    return (parser_now(parser)->kind == k);
}

Token* parser_now(Parser* parser) {
    if (parser->cursor < cjvec_len(parser->tokens)) {
	    return (Token*)cjvec_at(parser->tokens, parser->cursor);
    }
    // guaranteed to be eof_token
    return (Token*)cjvec_back(parser->tokens);
}

Token* parser_peek(Parser* parser) {
    if (parser->cursor + 1 < cjvec_len(parser->tokens)) {
	    return (Token*)cjvec_at(parser->tokens, parser->cursor + 1);
    }
    // guaranteed to be eof_token
    return (Token*)cjvec_back(parser->tokens);
}

Token* parser_before(Parser* parser) {
    if (parser->cursor > 0) {
	    return (Token*)cjvec_at(parser->tokens, parser->cursor - 1);
    }
    // guaranteed to be eof_token
    return (Token*)cjvec_back(parser->tokens);
}

Item* parse_extern(Parser* parser) {
    parser_advance(parser);

    if (match(parser, token_func_k)) {
        Item* func = parse_function(parser);
        func->data.fndef.is_extern = true;
        return func;
    }
    
    UNREACHABLE;
}

ParamDef parse_param(Parser* parser) {
    const char* param_name = NULL;
    TypeInfo*   param_type = NULL;
    Expr*       param_init = NULL;
    bool        has_init   = false;
    Span        param_span = {0};
    
    if (match(parser, token_ident_k)) {
        param_name = parser_now(parser)->text;
        param_span = get_span(parser);
        parser_advance(parser);
    } else {
        add_error(parser, make_syntax_error(get_span(parser),
        "expected a paramneter name", NULL));
        recover_until(parser, token_cparen_k);
        return err_param;
    }
    
    if (!expect(parser, token_colon_k)) {
        add_error(parser, make_syntax_error(get_span(parser),
        "expected a type", NULL));
        recover_until(parser, token_cparen_k);
        return err_param;
    }
    param_type = parse_type(parser);   

    if (match(parser, token_eq_k)) {
        parser_advance(parser);
        param_init = parse_expr(parser);
    }
        
    return (ParamDef) {param_name, has_init, param_span, param_type, param_init};
}

Item* parse_function(Parser* parser) {
    Span* start = get_pspan(parser);
    expect(parser, token_func_k);
    
    TypeInfo* return_type = type_new(type_none_k, "none", "void");
    FunctionDef funcdef = {0};
    
    if (match(parser, token_ident_k)) {
	    funcdef.name = parser_now(parser)->text;
	    parser_advance(parser);
    } else {
	    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Expected name after 'func'", NULL));
	    recover_until(parser, token_oparen_k);
    }

    if (match(parser, token_oparen_k)) {
	    parser_advance(parser);
        Params params = {0};
        while (!match(parser, token_cparen_k)) {

            if (match(parser, token_cvariadic_k)) {
                funcdef.is_variadic = true;
                parser_advance(parser);
                break;
            }
            
            ParamDef param = parse_param(parser);
            arr_append(&params, param, global_arena);
            if (match(parser, token_comma_k)) {
                parser_advance(parser);
            }
        }
        funcdef.params = params;
	    if (!expect(parser, token_cparen_k))  {
	        add_error(parser, make_syntax_error(get_span(parser), "unterminated parameter list", NULL));
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
    if (match(parser, token_semi_k)) {
        funcdef.is_decl = true;
        parser_advance(parser);
    } else {    
        parser->current_context = parse_func_body_k;
        funcdef.body = parse_expr(parser);
        parser->current_context = parse_context_no_set;
    }
    
    Span* end = get_pspan(parser);
    return item_make_fndef(funcdef, span_merge(start, end));
}

Stmt* parse_stmt(Parser* parser) {
    Stmt* stmt = NULL;
    if (match(parser, token_let_k)) {
        Span* start = get_pspan(parser);
        parser_advance(parser);

        VarDeclStmt vardecl = {0};
        
        vardecl.is_const = true;        
        vardecl.is_uninit = false; // always false for 'let' bindings
        
        if (match(parser, token_ident_k)) {
            vardecl.identifer = parser_now(parser)->text;
            parser_advance(parser);
        } else if (match(parser, token_oparen_k)) {
            TODO("Parser::parse_stmt::parse_vardecl: parse_pattern");
        } else {
            add_error(parser, make_syntax_error(*parser_now(parser)->span, "Unqualified token", "expected an identifier or pattern"));
            recover_until(parser, token_semi_k);
            return NULL;
        }

        if (match(parser, token_coleq_k)) {
            vardecl.type = type_new(type_any_k, "any", NULL);
            parser_advance(parser);
        } else if (match(parser, token_colon_k)){
            parser_advance(parser);
            vardecl.type = parse_type(parser);
            if (!expect(parser, token_eq_k)) {
                recover_until(parser, token_semi_k);
                return NULL;
            }
        } else {
            TODO("invalid syntax");
        }

        ParseContext prev_ctx = parser->current_context;
        parser->current_context = parse_vardecl_expr_k;
        vardecl.rhs = parse_expr(parser);
        parser->current_context = prev_ctx;
        
        Span* end = get_pspan(parser);
        if (!expect(parser, token_semi_k)) {
            err_semi(parser);
        }
        return stmt_make_vardecl(vardecl, span_merge(start, end));
    } else {
        // Try to parse an expr
        Span* start = get_pspan(parser);        
        Expr* expr  = parse_expr(parser);
        Span* end   = get_pspan(parser);

        if (!expect(parser, token_semi_k)) {
            err_semi(parser);
        }
        
        return stmt_make_expr(expr, span_merge(start, end));
    }
    TODO("Parser::parse_stmt: '%s'", parser_now(parser)->text);
    return stmt;
}

Item* parse_item(Parser* parser) {
    Item* item = NULL;
    if (match(parser, token_func_k)) {
	    item = parse_function(parser);
    } else if (match(parser, token_extern_k)) {
        item = parse_extern(parser);
    } else {
	    TODO("Parser::parse_item: other items");
    }
    return item;
}

Expr*  parse_expr(Parser* parser) {
    Expr* expr = parse_logical_or(parser);
    return expr;
}

Expr*  parse_logical_or(Parser* parser) {
    Expr* lhs = parse_logical_and(parser);
    return lhs;
}

Expr*  parse_logical_and(Parser* parser) {
    Expr* lhs = parse_equality(parser);
    return lhs;
}

Expr*  parse_equality(Parser* parser) {
    Expr* lhs = parse_conditional(parser);
    return lhs;
}

Expr*  parse_conditional(Parser* parser) {
    Expr* lhs = parse_additive(parser);
    return lhs;
}

Expr*  parse_additive(Parser* parser) {
    Span* start = get_pspan(parser);
    Expr* lhs = parse_term(parser);
    while (match(parser, token_add_k) || match(parser, token_sub_k)) {
        BinaryOpExpr binop = {0};
        binop.op = match(parser, token_add_k) ? BINOP_ADD: BINOP_SUB;
        parser_advance(parser);
        binop.lhs = lhs;
        binop.rhs = parse_term(parser);
        Span* end = get_pspan(parser);
        lhs = expr_make_binop(binop, span_merge(start, end));
    }    
    return lhs;
}

Expr* parse_term(Parser* parser) {
    Span* start = get_pspan(parser);
    Expr* lhs = parse_postifix(parser);
    while (match(parser, token_mul_k) || match(parser, token_div_k)) {
        BinaryOpExpr binop = {0};
        binop.op = match(parser, token_mul_k) ? BINOP_MUL: BINOP_DIV;
        parser_advance(parser);
        binop.lhs = lhs;
        binop.rhs = parse_term(parser);
        Span* end = get_pspan(parser);
        lhs = expr_make_binop(binop, span_merge(start, end));
    }
    return lhs;
}

Expr* parse_postifix(Parser* parser) {
    Span* start = get_pspan(parser);
    Expr* base  = parse_atom(parser);    
    
    while (1) {
        if (match(parser, token_oparen_k)) {
            parser_advance(parser);
            CJVec* args = cjvec_new(global_arena);
            while (!match(parser, token_cparen_k)) {
                Expr* arg = parse_expr(parser);
                cjvec_push(args, (void*)arg);
                if (match(parser, token_comma_k)) parser_advance(parser);
            }
            Span* end = get_pspan(parser);
            parser_advance(parser);
            base = expr_make_call(base, args, span_merge(start, end));
        } else {
            break;
        }
    }
    
    return base;
}

Expr* parse_atom(Parser* parser) {
    Expr* expr = NULL;
    if (match(parser, token_obrace_k)) {
	    parser_advance(parser);
	    expr = parse_body(parser);
	    if (!expect(parser, token_cbrace_k)) {
	        recover_until(parser, token_semi_k);
	        return NULL;
	    }
    } else if (match(parser, token_number_k)) {
        Span span = get_span(parser);
        long value = strtol(parser_now(parser)->text, NULL, 0);
        parser_advance(parser);
        return expr_make_literal_int(value, span);
    } else if (match(parser, token_ident_k)) {
        Span span = get_span(parser);
        const char* ident = parser_now(parser)->text;
        parser_advance(parser);
        return expr_make_identifier(ident, span);
    } else if (match(parser, token_cstring_k)) {
        Span span = get_span(parser);
        const char* value = parser_now(parser)->text;
        parser_advance(parser);
        return expr_make_cstring(value, span);
    } else if (match(parser, token_semi_k)) {
        Span span = get_span(parser);
        return expr_make_no_op(span);
    } else {
	    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Not a valid expression", NULL));	
	    if (parser->current_context == parse_func_body_k) recover_until(parser, token_eof_k);
        else if (parser->current_context == parse_vardecl_expr_k) recover_until(parser, token_semi_k);
	    return NULL;
    }
    return expr;
}

Expr* parse_body(Parser* parser) {
    CJVec* stmts = cjvec_new(global_arena);
    while (!match(parser, token_eof_k) && !match(parser, token_cbrace_k)) {
	    Stmt* stmt = parse_stmt(parser);
        cjvec_push(stmts, (void*)stmt);
    }    
    return expr_make_block(stmts);
}

TypeInfo* parse_type(Parser* parser) {
    if (match(parser, token_int_k)) {
        parser_advance(parser);
        return type_new(type_int_k, "int", NULL);
    } else if (match(parser, token_cstr_k)) {
        parser_advance(parser);
        return type_new(type_cstring_k, "cstr", NULL);
    } else if (match(parser, token_none_k)) {
        parser_advance(parser);
        return type_new(type_none_k, "none", NULL);
    } else {
        TODO("parser::parse_type: custom typenames");
    }
    
    add_error(parser, make_syntax_error(*parser_now(parser)->span, "Invalid type", NULL));   
    return NULL;
}

bool parser_parse(Parser* parser) {
    while (!match(parser, token_eof_k)) {
	    Item* item = parse_item(parser);
	    cjvec_push(parser->items, (void*)item);
    }
    
    return cjvec_len(parser->errors) == 0;
}

void parser_free(Parser* parser) {
    UNUSED(parser);
    // do nothing
    // structure lives on the arena
}
