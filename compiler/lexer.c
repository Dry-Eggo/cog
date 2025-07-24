#include <lexer.h>
#include <defines.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

lexer_t* lexer_new(compile_options_t* opts, const char* source) {
    lexer_t* lexer = jarena_alloc(global_arena, sizeof(lexer_t));

    lexer->source_path = opts->input_file;
    lexer->source = source;
    lexer->source_size = strlen(lexer->source);
    
    lexer->line = 1;
    lexer->column = 1;
    lexer->cursor = 0;

    lexer->tokens = jvec_new();

    return lexer;
}

void lexer_free(lexer_t* lexer) {
    if (lexer) {
	// doesn't free the tokens because they are allocated on the arena
	jvec_free(lexer->tokens);
    }
}

char lexer_now(lexer_t* lexer) {
    if (lexer->cursor < lexer->source_size) {
	return lexer->source[lexer->cursor];
    }
    return EOF;
}

char lexer_advance(lexer_t* lexer) {
    char ch = lexer_now(lexer);
    if (ch == '\n') {
	lexer->line++;
	lexer->column = 1;
    } else lexer->column++;
    lexer->cursor++;
    return ch;
}

token_t* lexer_parse_name(lexer_t* lexer) {
    juve_buffer_t* buffer = jb_create();
    size_t sl = lexer->line;
    size_t sc = lexer->column;
    
    while (lexer_now(lexer) != EOF && (isalpha(lexer_now(lexer)) || lexer_now(lexer) == '_')) {
	jb_appendf_a(buffer, global_arena, "%c", lexer_advance(lexer));
    }
    token_kind_t kind = token_ident_k;
    const char* text  = NULL;
    if (jb_eq(buffer, "func")) {
	kind = token_func_k;
	text = "func";
    } else if (jb_eq(buffer, "let")) {
	kind = token_let_k;
	text = "let";
    } else if (jb_eq(buffer, "var")) {
	kind = token_var_k;
	text = "var";
    } else {
	text = jb_str_a(buffer, global_arena);
    }
    span_t* span = span_new(sl, sc, lexer->column-1, lexer->source_path);
    token_t* tok = token_new(span, kind, jarena_strdup(global_arena, (char*)text));
    jb_free(buffer);
    return tok;
}

bool lexer_lex(lexer_t* lexer) {
    bool ok = true;
    while (lexer_now(lexer) != EOF) {

	if (isspace(lexer_now(lexer))) {
	    lexer_advance(lexer);
	    continue;
	}

	if (isalpha(lexer_now(lexer)) || lexer_now(lexer) == '_') {
	    token_t* tok = lexer_parse_name(lexer);
	    jvec_push(lexer->tokens, (void*)tok);
	    continue;
	}
	size_t sl = lexer->line;
	size_t sc = lexer->column;
	switch (lexer_now(lexer)) {
	case '(': {
	    lexer_advance(lexer);
	    token_t* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_oparen_k, "(");
	    jvec_push(lexer->tokens, (void*)tok);
	} break;
	case ')': {
	    lexer_advance(lexer);
	    token_t* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_cparen_k, ")");
	    jvec_push(lexer->tokens, (void*)tok);
	} break;
	case '{': {
	    lexer_advance(lexer);
	    token_t* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_obrace_k, "{");
	    jvec_push(lexer->tokens, (void*)tok);
	} break;
	case '}': {
	    lexer_advance(lexer);
	    token_t* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_cbrace_k, "}");
	    jvec_push(lexer->tokens, (void*)tok);
	} break;
	default:
	    log_err("(%s:%ld:%ld) Invalid Character: '%c'\n", lexer->source_path, lexer->line, lexer->column, lexer_now(lexer));
	    ok = false;
	    lexer_advance(lexer);
	    break;
	}
    }
    
    token_t* tok = token_new(span_new(lexer->line, lexer->column, lexer->column, lexer->source_path), token_eof_k, "<eof>");
    jvec_push(lexer->tokens, (void*)tok);
    return ok;
}
