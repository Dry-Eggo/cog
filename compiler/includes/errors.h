#pragma once
#include <token.h>

typedef struct {
    span_t span;
    const char* message;
    const char* hint;
} syntax_error_t;

typedef enum {
    sema_error_undeclared_variable_k,
    sema_error_unknown_type_k,
    sema_error_invalid_type_k,
} sema_error_kind_t;

typedef struct error_unknown_type unknown_type_e;
typedef struct error_undeclared_variable undeclared_var_e;
typedef struct error_invalid_type invalid_type_e;

typedef struct {
    sema_error_kind_t kind;

    union {
        struct error_unknown_type {
            span_t span;
            const char* type_str;

            // for later suggested types 'did you mean _ ?'
            const char* suggested_type;
        } un_ty;

        struct error_invalid_type {
            span_t span;
            const char* expected;
            const char* got;
        } inv_ty;

        struct error_undeclared_variable {
            span_t span;
            const char* name;
        } un_var;
    } as;
    
} sema_error_t;

syntax_error_t* make_syntax_error(span_t span, const char* msg, const char* hint);
sema_error_t*   make_undeclared_var(span_t span, const char* name);
sema_error_t*   make_invalid_type(span_t span, const char* got, const char* expected);
void syntax_error_flush(cjvec_t* errors, juve_vec_t* source);
void sema_error_flush(cjvec_t* errors, juve_vec_t* source);
