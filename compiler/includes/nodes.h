#pragma once

#include <enum.h>
#include <token.h>
#include <types.h>
#include <compiler.h>

typedef struct expr_ Expr;
typedef struct stmt_ Stmt;

typedef struct expr_literal {
    Span span;
    literal_kind_t kind;
    union {
	    const char* str_value;
        const char* identifier_value;
        const char* cstring_value;
        
	    long int_value;
	    bool bool_value;
	    char char_value;
	    float float_value;
    } lit;
} LiteralExpr;

typedef struct stmt_vardecl {
    Span span;
    var_decl_kind_t kind;    
    bool is_uninit;
    bool is_const;

    union {
        const char* identifer;
        Expr*     pattern;
    };

    TypeInfo* type;
    Expr* rhs;
} VarDeclStmt;

typedef struct item_funcdef {
    bool is_extern;
    bool is_decl;
    
    Span name_span;
    const char* name;
    const char* linkage_name;
    
    JVec*   params; // vector of param_t*
    Expr*     body;
    TypeInfo*     return_type;
} FunctionDef;

typedef struct expr_block {
    CJVec* statements;
} BlockExpr;

typedef struct expr_binop {
    enum { BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV } op;
    Expr* lhs;
    Expr* rhs;
} BinaryOpExpr;

typedef struct expr_inner {
    Expr* inner;
} InnerExpr;

struct expr_ {
    expr_kind_t kind;
    Span span;
    union {
	    struct expr_literal literal;
	    struct expr_block   block;
        struct expr_binop   binop;
        struct expr_inner   inner_expr;
    } data;
};

struct stmt_ {
    stmt_kind_t kind;
    Span span;
    union {
	    struct stmt_vardecl vardecl;
	    struct expr_ expr;
    } data;
};

typedef struct {
    item_kind_t kind;
    Span span;
    union {
	    struct item_funcdef fndef;
    } data;
} Item;

Expr* expr_make_block(CJVec* stmts);
Expr* expr_make_literal_int(long value, Span span);
Expr* expr_make_identifier(const char* name, Span span);
Expr* expr_make_binop(BinaryOpExpr expr, Span span);
Expr* expr_make_cstring(const char* value, Span span);
Stmt* stmt_make_vardecl(VarDeclStmt vardecl, Span);

Item* item_make_fndef(FunctionDef fn, Span span);
