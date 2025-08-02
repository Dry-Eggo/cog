#include <lexer.h>
#include <defines.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

Lexer* lexer_new(CompileOptions* opts, const char* source) {
    Lexer* lexer = jarena_alloc(global_arena, sizeof(Lexer));

    lexer->source_path = opts->input_file;
    lexer->source = source;
    lexer->source_size = strlen(lexer->source);
    
    lexer->line = 1;
    lexer->column = 1;
    lexer->cursor = 0;

    lexer->tokens = cjvec_new(global_arena);

    return lexer;
}

void lexer_free(Lexer* lexer) {
}

char lexer_now(Lexer* lexer) {
    if (lexer->cursor < lexer->source_size) {
	    return lexer->source[lexer->cursor];
    }
    return EOF;
}

char lexer_get(Lexer* lexer, size_t n) {
    if (lexer->cursor + (n) < lexer->source_size) {
	    return lexer->source[lexer->cursor + (n)];
    }
    return EOF;  
}

char lexer_advance(Lexer* lexer) {
    char ch = lexer_now(lexer);
    if (ch == '\n') {
	    lexer->line++;
	    lexer->column = 1;
    } else lexer->column++;
    lexer->cursor++;
    return ch;
}

Token* lexer_parse_name(Lexer* lexer) {
    CJBuffer* buffer = cjb_create(global_arena);
    size_t sl = lexer->line;
    size_t sc = lexer->column;
    
    while (lexer_now(lexer) != EOF && (isalnum(lexer_now(lexer)) || lexer_now(lexer) == '_')) {
	    cjb_appendf(buffer, "%c", lexer_advance(lexer));
    }
    TokenKind kind = token_ident_k;
    const char* text  = NULL;
    if (cjb_eq(buffer, "func")) {
	    kind = token_func_k;
	    text = "func";
    } else if (cjb_eq(buffer, "extern")) {
	    kind = token_extern_k;
	    text = "extern";
    } else if (cjb_eq(buffer, "let")) {
	    kind = token_let_k;
	    text = "let";
    } else if (cjb_eq(buffer, "var")) {
	    kind = token_var_k;
	    text = "var";
    } else if (cjb_eq(buffer, "int")) {
        kind = token_int_k;
        text = "int";
    } else if (cjb_eq(buffer, "none")) {
        kind = token_none_k;
        text = "none";
    } else if (cjb_eq(buffer, "cstr")) {
        kind = token_cstr_k;
        text = "cstr";
    } else {
	    text = cjb_str(buffer);
    }
    Span* span = span_new(sl, sc, lexer->column-1, lexer->source_path);
    Token* tok = token_new(span, kind, jarena_strdup(global_arena, (char*)text));
    return tok;
}

bool lexer_lex(Lexer* lexer) {
    bool ok = true;
    while (lexer_now(lexer) != EOF) {
	    if (isspace(lexer_now(lexer))) {
	        lexer_advance(lexer);
	        continue;
	    }

        if (lexer_now(lexer) == 'c' && lexer_get(lexer, 1) == '"') {
            lexer_advance(lexer); lexer_advance(lexer);
            JBuffer* jb = jb_create();

            size_t sl = lexer->line;
            size_t sc = lexer->column;
            
            while (lexer_now(lexer) != EOF && lexer_now(lexer) != '"') {
                jb_appendf_a(jb, global_arena, "%c", lexer_advance(lexer));
            }
            
            ASSERT(lexer_now(lexer) != EOF, "Handle Unexpected EOF while lexing");
            
            lexer_advance(lexer); // skip trailing '"'
            
            const char* text = jb_str_a(jb, global_arena);
            
            Span* span = span_new(sl, sc, lexer->column-1, lexer->source_path);
            
            Token* tok = token_new(span, token_cstring_k, jarena_strdup(global_arena, (char*)text));   
            cjvec_push(lexer->tokens, (void*)tok);
            
            jb_free(jb);
            
            continue;
        }
        
	    if (isalpha(lexer_now(lexer)) || lexer_now(lexer) == '_') {
	        Token* tok = lexer_parse_name(lexer);
	        cjvec_push(lexer->tokens, (void*)tok);
	        continue;
	    }

        if (isdigit(lexer_now(lexer))) {
            JBuffer* buffer = jb_create();
            size_t sl = lexer->line;
            size_t sc = lexer->column;
            TokenKind kind = token_number_k;
            
            while (isdigit(lexer_now(lexer))) {
                jb_appendf_a(buffer, global_arena, "%c", lexer_advance(lexer));
            }

            const char* text = jb_str_a(buffer, global_arena);
            Span* span = span_new(sl, sc, lexer->column-1, lexer->source_path);
            Token* tok = token_new(span, kind, jarena_strdup(global_arena, (char*)text));
            cjvec_push(lexer->tokens, (void*)tok);
            jb_free(buffer);
            continue;
        }
        
	    size_t sl = lexer->line;
	    size_t sc = lexer->column;
	    switch (lexer_now(lexer)) {
        case ':': {
            lexer_advance(lexer);
            if (lexer_now(lexer) == '=') {
                lexer_advance(lexer);
                Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_coleq_k, ":=");
	            cjvec_push(lexer->tokens, (void*)tok);
                continue;
            }
            Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_colon_k, ":");
	        cjvec_push(lexer->tokens, (void*)tok);
            continue;
        } break;
        case '-': {
            lexer_advance(lexer);
            if (lexer_now(lexer) == '>') {
                lexer_advance(lexer);
                Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_slim_arrow_k, "->");
	            cjvec_push(lexer->tokens, (void*)tok);
                continue;
            }
            Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_sub_k, "-");
	        cjvec_push(lexer->tokens, (void*)tok);
            continue;
        } break;
        case ';': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_semi_k, ";");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
        case '=': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_eq_k, "=");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
	    case '(': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_oparen_k, "(");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
	    case ')': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_cparen_k, ")");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
	    case '{': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_obrace_k, "{");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
	    case '}': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_cbrace_k, "}");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
        case '+': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_add_k, "+");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
        case ',': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_comma_k, ",");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
        case '.': {
            if (lexer_get(lexer, 1) == '.' && lexer_get(lexer, 2) == '.') {
                lexer_advance(lexer); lexer_advance(lexer); lexer_advance(lexer);
	            Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_cvariadic_k, "...");
	            cjvec_push(lexer->tokens, (void*)tok);
                continue;
            }
            TODO("lex '.'");
	    } break;
        case '*': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_mul_k, "*");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
        case '/': {
	        lexer_advance(lexer);
	        Token* tok = token_new(span_new(sl, sc, lexer->column-1, lexer->source_path), token_div_k, "/");
	        cjvec_push(lexer->tokens, (void*)tok);
	    } break;
	    default:
	        LOG_ERR("(%s:%ld:%ld) Invalid Character: '%c'\n", lexer->source_path, lexer->line, lexer->column, lexer_now(lexer));
	        ok = false;
	        lexer_advance(lexer);
	        break;
	    }
    }
    
    Token* tok = token_new(span_new(lexer->line, lexer->column, lexer->column, lexer->source_path), token_eof_k, "<eof>");
    cjvec_push(lexer->tokens, (void*)tok);
    return ok;
}
