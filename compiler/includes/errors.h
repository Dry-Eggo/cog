#pragma once
#include <token.h>
#include <compiler.h>

typedef struct {
    Span span;
    const char* message;
    const char* hint;
} SyntaxError;

typedef enum {
    SemaErrorUndeclaredVariable,
    SemaErrorUnknownType,
    SemaErrorInvalidType,
    SemaErrorInvalidOperandType,
    SemaErrorInvalidBinaryOperand,
} SemaErrorKind;

typedef struct error_unknown_type UnknownTypeError;
typedef struct error_undeclared_variable UndeclaredVarError;
typedef struct error_invalid_type InvalidTypeError;
typedef struct error_operand_type_mismatch OperandTypeMismatch;
typedef struct error_invalid_binary_operand InvalidBinaryOperand;

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

        struct error_invalid_binary_operand {
            Span span;
            const char* type_str;
        } inv_bin_opr;

        struct error_operand_type_mismatch {
            Span op1;
            Span op2;

            const char* op1_ty;
            const char* op2_ty;
        } inv_op;
    } as;
    
} SemaError;

SyntaxError* make_syntax_error(Span span, const char* msg, const char* hint);

SemaError*   make_undeclared_var(Span span, const char* name);
SemaError*   make_invalid_binary_operand(Span span, const char* type_str);
SemaError*   make_invalid_type(Span span, const char* got, const char* expected);
SemaError*   make_invalid_operand_type(Span op1, Span op2, const char* op1_ty, const char* op2_ty);

void syntax_error_flush(CJVec* errors, JVec* source);
void sema_error_flush(CJVec* errors, JVec* source);
