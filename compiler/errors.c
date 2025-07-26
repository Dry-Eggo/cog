#include <errors.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <defines.h>
#include <math.h>

#define MAX_GUTTER_WIDTH 5

syntax_error_t* make_syntax_error(span_t span, const char* msg, const char* hint) {
    syntax_error_t* err = (syntax_error_t*)jarena_alloc(global_arena, sizeof(syntax_error_t));
    err->span = span;
    err->message = msg;
    err->hint = hint;
    return err;
}

sema_error_t*   make_undeclared_var(span_t span, const char* name) {
    sema_error_t* err = alloc(sema_error_t);
    err->kind = sema_error_undeclared_variable_k;
    err->as.un_var.span = span;
    err->as.un_var.name = name;
    return err;
}

sema_error_t*   make_invalid_type(span_t span, const char* got, const char* expected) {
    sema_error_t* err = alloc(sema_error_t);
    err->kind = sema_error_invalid_type_k;
    err->as.inv_ty.span = span;
    err->as.inv_ty.got = got;
    err->as.inv_ty.expected = expected;
    return err;    
}


void underline_span(span_t span, const char* line) {
    int fc = 0;
    while (isspace(line[fc])) {
	fc++;
	fprintf(stderr, " ");
    }
    for (int i = fc; i < strlen(line); ++i) {
	if (i >= span.column - 1 && i <= span.offset - 1) {
	    if (i == span.column - 1) {
		fprintf(stderr, "^");
	    } else {
		fprintf(stderr, "~");
	    }
	} else {
	    fprintf(stderr," ");
	}
    }
}

void print_gutter_line(bool nl) {
    fprintf(stderr, " %*s |%s", MAX_GUTTER_WIDTH, "", nl? "\n": "");
}

void print_gutter_with_line(size_t lineno, const char* line) {
    fprintf(stderr, " %5.zu |%s\n", lineno, line);
}

void report_err(syntax_error_t* err, const char* line) {
    span_t span = err->span;
    fprintf(stderr, "%sKudo%s: %s%sSyntax Error%s: (%ld:%ld): %s\n", c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err->message);
    fprintf(stderr, "    ----> %s\n", span.filename);
    fprintf(stderr, "   |\n");
    fprintf(stderr, " %ld |%s\n", span.line, line);
    fprintf(stderr, "   |");
    underline_span(span, line);
    if (err->hint) {
        fprintf(stderr, ": %s%s%s", c(color_blue_k), err->hint, c(color_reset_k));
    }
    fprintf(stderr,"\n");
}

void syntax_error_flush(cjvec_t* errors, juve_vec_t* source) {
    int err_count = cjvec_len(errors);
    for (int i = 0; i < err_count; ++i) {
	    syntax_error_t* err = (syntax_error_t*)cjvec_at(errors, i);
	    const char* line = jvec_at(source, err->span.line - 1);
	    report_err(err, line);
    }
}

void format_err_inv_ty(invalid_type_e err, juve_vec_t* source) {
    span_t span = err.span;
    const char* line1 = span.line - 1 <= 1 ? NULL: jvec_at(source, span.line - 2);
    const char* line2 = jvec_at(source, span.line - 1);
    fprintf(stderr, "%sKudo%s: %s%sType Error%s: (%ld:%ld): cannot implicitly convert from '%s' to '%s'\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.got, err.expected);

    print_gutter_line(true);
    if (line1) {
        print_gutter_with_line(span.line - 1, line1);
    }
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span(span, line2);
    fprintf(stderr, "\n");
}

void format_err_un_var(undeclared_var_e err, juve_vec_t* source) {
    span_t span = err.span;
    const char* line1 = span.line - 1 <= 1 ? NULL: jvec_at(source, span.line - 2);
    const char* line2 = jvec_at(source, span.line - 1);
    fprintf(stderr, "%sKudo%s: %s%sError%s: (%ld:%ld): symbol '%s' was not declared in this scope\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.name);

    print_gutter_line(true);
    if (line1) {
        print_gutter_with_line(span.line - 1, line1);
    }
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span(span, line2);
    fprintf(stderr, "\n");
}

void sema_error_flush(cjvec_t* errors, juve_vec_t* source) {
    fori(sema_error_t*, err, i, errors) {
        switch(err->kind) {
        case sema_error_invalid_type_k: {
            format_err_inv_ty(err->as.inv_ty, source);
        } break;
        case sema_error_undeclared_variable_k: {
            format_err_un_var(err->as.un_var, source);
        } break;
        default:
        unreachable;
    }
}
}
