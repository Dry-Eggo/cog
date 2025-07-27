#pragma once
#include <token.h>
#include <compiler.h>

typedef struct {
    Span span;
    const char* message;
    const char* hint;
} SyntaxError;

typedef enum {
    sema_error_undeclared_variable_k,
    sema_error_unknown_type_k,
    sema_error_invalid_type_k,
} SemaErrorKind;

typedef struct error_unknown_type UnknownTypeError;
typedef struct error_undeclared_variable UndeclaredVarError;
typedef struct error_invalid_type InvalidTypeError;

typedef struct {
    SemaErrorKind kind;

    union {
        struct error_unknown_type {
            Span span;
            const char* type_str;

            // for later suggested types 'did you mean _ ?'
            const char* suggested_type;
        } un_ty;

        struct error_invalid_type {
            Span span;
            const char* expected;
            const char* got;
        } inv_ty;

        struct error_undeclared_variable {
            Span span;
            const char* name;
        } un_var;
    } as;
    
} sema_error_t;

SyntaxError* make_syntax_error(Span span, const char* msg, const char* hint);
sema_error_t*   make_undeclared_var(Span span, const char* name);
sema_error_t*   make_invalid_type(Span span, const char* got, const char* expected);
void syntax_error_flush(CJVec* errors, JVec* source);
void sema_error_flush(CJVec* errors, JVec* source);
