#include <errors.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <defines.h>
#include <math.h>

#define MAX_GUTTER_WIDTH 5

SyntaxError* make_syntax_error(Span span, const char* msg, const char* hint) {
    SyntaxError* err = (SyntaxError*)jarena_alloc(global_arena, sizeof(SyntaxError));
    err->span = span;
    err->message = msg;
    err->hint = hint;
    return err;
}

SemaError*   make_undeclared_var(Span span, const char* name) {
    SemaError* err = ALLOC(SemaError);
    err->kind = SemaErrorUndeclaredVariable;
    err->as.un_var.span = span;
    err->as.un_var.name = name;
    return err;
}

SemaError*   make_invalid_type(Span span, const char* got, const char* expected) {
    SemaError* err = ALLOC(SemaError);
    err->kind = SemaErrorInvalidType;
    err->as.inv_ty.span = span;
    err->as.inv_ty.got = got;
    err->as.inv_ty.expected = expected;
    return err;    
}

SemaError* make_invalid_binary_operand(Span span, const char* type_str) {
    SemaError* err = ALLOC(SemaError);
    err->kind = SemaErrorInvalidBinaryOperand;
    err->as.inv_bin_opr.span = span;
    err->as.inv_bin_opr.type_str = type_str;
    return err;    
}

SemaError* make_invalid_operand_type(Span op1, Span op2, const char* op1_ty, const char* op2_ty) {
    SemaError* err = ALLOC(SemaError);
    err->kind = SemaErrorInvalidOperandType;
    err->as.inv_op.op1 = op1;
    err->as.inv_op.op2 = op2;
    err->as.inv_op.op1_ty = op1_ty;
    err->as.inv_op.op2_ty = op2_ty;
    return err;
}

SemaError*  make_generic_error(Span span, const char* message, const char* hint) {
    SemaError* err = ALLOC(SemaError);
    err->kind = SemaErrorGeneric;
    err->as.gen.span = span;
    err->as.gen.message = message;
    err->as.gen.hint = hint;
    return err;
}

void underline_span(Span span, const char* line) {
    int fc = 0;
    while (isspace(line[fc])) {
	    fprintf(stderr, "%c", line[fc]);
        fc++;
    }
    
    for (size_t i = fc; i < strlen(line); ++i) {
	    if (i >= span.column - 1 && i <= span.offset - 1) {
	        if (i == span.column - 1) {
		        fprintf(stderr, "^");
	        } else {
		        fprintf(stderr, "~");
	        }
	    } else {
            fprintf(stderr, " ");
	    }
    }
}

void underline_span2(Span span, Span span2, const char* line) {
    int fc = 0;
    while (isspace(line[fc])) {
	    fprintf(stderr, "%c", line[fc]);
	    fc++;
    }
    
    size_t last = 0;
    for (last = fc; last < span.offset; ++last) {
	    if (last >= span.column - 1 && last <= span.offset - 1) {
	        if (last == span.column - 1) {
		        fprintf(stderr, "^");
	        } else {
		        fprintf(stderr, "~");
	        }
	    } else {
            fprintf(stderr," ");
	    }
    }
    
    for (; last < strlen(line); ++last) {
	    if (last >= span2.column - 1 && last <= span2.offset - 1) {
	        if (last == span2.column - 1) {
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

void report_err(SyntaxError* err, const char* line, JVec* source) {
    Span span = err->span;

    const char* prev_line = span.line - 1 > 1 ? jvec_at(source, span.line - 2) : NULL;
    
    fprintf(stderr, "%sCog%s: %s%sSyntax Error%s: (%ld:%ld): %s\n", c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err->message);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_line(true);
    if (prev_line) {
        print_gutter_with_line(span.line - 1, prev_line);
    }
    print_gutter_with_line(span.line, line);
    print_gutter_line(false);
    underline_span(span, line);
    if (err->hint) {
        fprintf(stderr, " : %s%s%s", c(color_blue_k), err->hint, c(color_reset_k));
    }
    fprintf(stderr,"\n");
}

void syntax_error_flush(JVec errors, JVec source) {
    int err_count = jvec_len(errors);
    for (int i = 0; i < err_count; ++i) {
	    SyntaxError* err = (SyntaxError*)jvec_at(&errors, i);
	    const char* line = jvec_at(&source, err->span.line - 1);
	    report_err(err, line, &source);
    }
}

void format_err_inv_ty(InvalidTypeError err, JVec* source) {
    Span span = err.span;
    const char* line1 = span.line - 1 <= 1 ? NULL: jvec_at(source, span.line - 2);
    const char* line2 = jvec_at(source, span.line - 1);
    fprintf(stderr, "%sCog%s: %s%sType Error%s: (%ld:%ld): cannot implicitly convert from '%s' to '%s'\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.got, err.expected);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_line(true);
    if (line1) {
        print_gutter_with_line(span.line - 1, line1);
    }
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span(span, line2);
    fprintf(stderr, "\n");
}

void format_err_un_var(UndeclaredVarError err, JVec* source) {
    Span span = err.span;
    const char* line1 = span.line - 1 <= 1 ? NULL: jvec_at(source, span.line - 2);
    const char* line2 = jvec_at(source, span.line - 1);
    fprintf(stderr, "%sCog%s: %s%sError%s: (%ld:%ld): symbol '%s' was not declared in this scope\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.name);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_line(true);
    if (line1) {
        print_gutter_with_line(span.line - 1, line1);
    }
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span(span, line2);
    fprintf(stderr, "\n");
}

void format_err_inv_op(OperandTypeMismatch err, JVec* source) {
    Span span = err.op1;   
    const char* line2 = jvec_at(source, span.line - 1);
    
    fprintf(stderr, "%sCog%s: %s%sError%s: (%ld:%ld): invalid operation between '%s' and '%s'\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.op1_ty, err.op2_ty);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_line(true);
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span2(span, err.op2, line2);
    fprintf(stderr, "\n");    
}

void format_err_inv_bin_opr(InvalidBinaryOperand err, JVec* source) {
    Span span = err.span;
    
    const char* line1 = span.line - 1 <= 1 ? NULL: jvec_at(source, span.line - 2);
    const char* line2 = jvec_at(source, span.line - 1);
    
    fprintf(stderr, "%sCog%s: %s%sError%s: (%ld:%ld): cannot perform operation on type '%s'\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.type_str);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_line(true);
    if (line1) {
        print_gutter_with_line(span.line - 1, line1);
    }
    print_gutter_with_line(span.line, line2);
    print_gutter_line(false);
    underline_span(span, line2);
    fprintf(stderr, "\n");   
}

void format_err_generic(GenericSema err, JVec* source) {
    Span span = err.span;
    const char* line = jvec_at(source, span.line - 1);
        fprintf(stderr, "%sCog%s: %s%sError%s: (%ld:%ld): %s\n",
    c(color_bold_k), c(color_reset_k), c(color_bold_k),
    c(color_red_k), c(color_reset_k), span.line, span.column, err.message);
    fprintf(stderr, "    ----> %s\n", span.filename);
    print_gutter_with_line(span.line, line);
    print_gutter_line(false);
    underline_span(span, line);
    fprintf(stderr, "\n");   
}

void sema_error_flush(JVec errors, JVec source) {
    FOREACH(SemaError*, err, i, errors) {
        switch(err->kind) {
        case SemaErrorInvalidType: {
            format_err_inv_ty(err->as.inv_ty, &source);
        } break;
        case SemaErrorUndeclaredVariable: {
            format_err_un_var(err->as.un_var, &source);
        } break;
        case SemaErrorInvalidOperandType: {
            format_err_inv_op(err->as.inv_op, &source);
        } break;
        case SemaErrorInvalidBinaryOperand: {
            format_err_inv_bin_opr(err->as.inv_bin_opr, &source);
        } break;
        case SemaErrorGeneric: {
            format_err_generic(err->as.gen, &source);
        } break;
        default:
            UNREACHABLE;
        }
        if (i != jvec_len(errors) - 1) fprintf(stderr, "\n");
    }
}
