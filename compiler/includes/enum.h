#pragma once

typedef enum {
    // keywords
    token_func_k,
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
    
    token_eof_k,
} token_kind_t;

typedef enum {
    var_decl_ident,
    var_decl_pattern
} var_decl_kind_t;

typedef enum {
    literal_int_k,
    literal_bool_k,
    literal_float_k,
    literal_str_k,
} literal_kind_t;

typedef enum {
    expr_binop_k,
    expr_ident_k,
    expr_string_k,
    expr_cstring_k,
    expr_int_k,
    expr_funccall_k,
    expr_compound_stmt_k,
    expr_error,
} expr_kind_t;

typedef enum {
    item_function_k,
    item_enum_k,
    item_struct_def_k,
    item_record_def_k,
    item_import_k,
    item_module_k,
} item_kind_t;

typedef enum {
    stmt_function_k,
    stmt_vardecl_k,
    stmt_expr_k,
    stmt_assign_k,
} stmt_kind_t;

typedef enum {
    color_reset_k,
    color_bold_k,
    color_red_k,
    color_yello_k,
    color_blue_k,
    color_count_k
} color_t;
