#pragma once

#include <enum.h>
#include <token.h>

typedef struct expr_ expr_t;
typedef struct stmt_ stmt_t;

typedef struct expr_literal {
    span_t span;
    literal_kind_t kind;
    union {
	const char* str_value; /* used for both identifers and any value that requires string storage */
	long int_value;
	bool bool_value;
	char char_value;
	float float_value;
    } lit;
} literal_t;

typedef struct stmt_vardecl {
    var_decl_kind_t kind;
    bool is_uninit;
    bool is_const;

    expr_t* identifier; // could be a pattern or literal
    expr_t* rhs;
} vardecl_t;

typedef struct item_funcdef {
    bool is_extern;
    bool is_decl;
    
    span_t name_span;
    const char* name;
    const char* linkage_name;
    
    juve_vec_t*   params; // vector of param_t*
    expr_t*     body;
} funcdef_t;

typedef struct expr_block {
    juve_vec_t* statements;
} block_t;

struct expr_ {
    expr_kind_t kind;
    span_t span;
    union {
	struct expr_literal literal;
	struct expr_block   block;
    } data;
};

struct stmt_ {
    stmt_kind_t kind;
    span_t span;
    union {
	struct stmt_vardecl vardecl;
	struct expr_ expr;
    } data;
};

typedef struct {
    item_kind_t kind;
    span_t span;
    union {
	struct item_funcdef fndef;
    } data;
} item_t;

expr_t* expr_make_block(juve_vec_t* stmts);
