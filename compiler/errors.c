#include <errors.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <defines.h>

syntax_error_t* make_syntax_error(span_t span, const char* msg, const char* hint) {
    syntax_error_t* err = (syntax_error_t*)jarena_alloc(global_arena, sizeof(syntax_error_t));
    err->span = span;
    err->message = msg;
    err->hint = hint;
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
    fprintf(stderr,"\n");
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
}

void syntax_error_flush(juve_vec_t* errors, juve_vec_t* source) {
    int err_count = jvec_len(errors);
    for (int i = 0; i < err_count; ++i) {
	syntax_error_t* err = (syntax_error_t*)jvec_at(errors, i);
	const char* line = jvec_at(source, err->span.line - 1);
	report_err(err, line);
    }
}
