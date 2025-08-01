#pragma once

typedef enum {
    // keywords
    token_func_k,
    token_extern_k,
    token_return_k,
    token_let_k,
    token_var_k,
    token_int_k,
    token_cstr_k,
    token_str_k,
    token_none_k,
    
    // values
    token_number_k,
    token_string_k,
    token_cstring_k,
    token_float_k,
    token_ident_k,

    // punctuations
    token_oparen_k,
    token_cparen_k,
    token_obrace_k,
    token_cbrace_k,
    token_semi_k,
    token_colon_k,
    token_slim_arrow_k, // ->
    token_fat_arrow_k,  // =>
    token_comma_k,      // ,
    token_coleq_k,      // :=
    token_eq_k,
    token_cvariadic_k,  // ...

    // operators
    token_add_k,     // +
    token_sub_k,     // -
    token_mul_k,     // *
    token_div_k,     // /
    token_inc_k,     // ++
    token_dec_k,     // --
    token_mul_eq_k,  // *=
    token_div_eq_k,  // /=
    token_shl_k,     // <<
    token_shr_k,     // >>
    token_shl_eq_k,  // <<=
    token_shr_eq_k,  // >>=
    token_eqeq_k,    // ==
    token_or_k,      // ||
    token_bor_k,     // |
    token_and_k,     // &&
    token_band_k,    // &
    token_and_eq_k,  // &=
    token_oreq_k,    // |=
    
    token_eof_k,
} TokenKind;

typedef enum {
    var_decl_ident,
    var_decl_pattern
} VarDeclKind;

typedef enum {
    literal_int_k,
    literal_bool_k,
    literal_float_k,
    literal_str_k,
} LiteralKind;

typedef enum {
    expr_binop_k,
    expr_ident_k,
    expr_string_k,
    expr_cstring_k,
    expr_int_k,
    expr_funccall_k,
    expr_compound_stmt_k,
    expr_empty_k,
    
    expr_error,
} ExprKind;

typedef enum {
    item_function_k,
    item_enum_k,
    item_struct_def_k,
    item_record_def_k,
    item_import_k,
    item_module_k,
} ItemKind;

typedef enum {
    stmt_function_k,
    stmt_vardecl_k,
    stmt_expr_k,
    stmt_assign_k,
} StmtKind;

typedef enum {
    color_reset_k,
    color_bold_k,
    color_red_k,
    color_yello_k,
    color_blue_k,
    color_count_k
} Color;
