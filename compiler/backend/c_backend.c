#include <c_backend.h>
#include <compiler.h>
#include <defines.h>
#include <juve/misc.h>

#define CCTX_ALLOC(ctx, ty) (ty*)jarena_alloc((ctx)->arena, sizeof(ty))

typedef jtab_tracker_t JTab;

typedef struct CIntType CIntType;
typedef struct CItem CItem;
typedef struct CBlock CBlock;

typedef enum {
    CItemFunction,
    CItemStruct,
    CItemTypeDef
} CItemKind;

typedef enum {
    CExprInt,
    CExprString,
    CExprChar,
    CExprIdentifier,
    CExprBinaryOp,
} CExprKind;

typedef enum {
    CStmtReturn,
    CStmtAssignVar,
    CStmtDeclareVar,
} CStmtKind;

typedef enum {
    CBinaryAdd,
    CBinarySub,
} CBinaryOp;

struct CStmt {
    CStmtKind kind;
    union {
        struct { CExpr* expr; } return_stmt;
        struct { const char* name; CExpr* expr; CType* type; } assign;
    };
};

struct CExpr {
    CExprKind kind;
    CType* type;
    union {
        int64_t int_expr;
        const char* string_expr;
        char char_expr;
        bool bool_expr;
        struct { CExpr* lhs; CExpr* rhs; CBinaryOp op; } binop;
    };
};

struct CItem {
    CItemKind kind;
    void* data;
};

struct CIntType {
    CIntSign sign;
};

struct CBlock {
    CBlock* parent;
    CJVec* statements;    
};

struct CContext {
    JBuffer* includes; // includes
    JBuffer* header;   // foward_decl    
    JBuffer* body;     // main code

    bool     output_built;
    JBuffer* final_output;
    
    CBlock*  current_block;
    
    CJVec*   items;

    JTab tab_tracker;
    JArena*  arena;
};

struct CParam {    
    const char* name;
    CType*      type;
};

struct CFunction {
    const char* name;
    CType*      type;    
    CJVec*      params;

    CExpr*      return_value;
    
    CBlock*     block;    
    bool        has_definition;
    bool        is_extern;
    bool        is_variadic;
};

struct CType {
    CTypeKind kind;
    bool      const_;

    union {
        CIntType int_ty;
    };
};


CContext* cctx_new(JArena* arena) {
    CContext* ctx = ALLOC(CContext);
    ctx->includes = jb_create();
    ctx->header   = jb_create();
    ctx->body     = jb_create();

    ctx->items = cjvec_new(global_arena);
    
    ctx->output_built = false;    
    
    ctx->arena = arena;
    ctx->tab_tracker = jtab_new(arena);
    return ctx;
}

CItem* cctx_make_item(CContext* cctx, CItemKind kind, void* data) {
    CItem* item = CCTX_ALLOC(cctx, CItem);
    item->kind = kind;
    item->data = data;
    return item;
}


CBlock* cctx_new_block(CContext* cctx, CBlock* parent) {
    CBlock* block = CCTX_ALLOC(cctx, CBlock);
    block->statements = cjvec_new(cctx->arena);
    block->parent = parent;
    return block;
}

void cctx_end_block(CContext* cctx) {
    CBlock* prev_block = cctx->current_block;
    if (prev_block->parent) {
        cctx->current_block = prev_block->parent;
    } else {
        cctx->current_block = NULL;
    }
}

CParam* cctx_new_parameter(CContext* cctx, const char* name, CType* type) {
    CParam* param = CCTX_ALLOC(cctx, CParam);
    param->name = jarena_strdup(cctx->arena, (char*)name);
    param->type = type;
    return param;
}

void cctx_function_set_extern(CContext* cctx, CFunction* func) {
    UNUSED(cctx);
    func->is_extern = true;
}

void cctx_function_set_variadic(CContext* cctx, CFunction* func) {
    UNUSED(cctx);
    func->is_variadic = true;
}

CFunction* cctx_create_function(CContext* cctx, const char* name, CJVec* params, CType* type, bool has_definition) {
    CFunction* func = CCTX_ALLOC(cctx, CFunction);
    func->name = name;
    func->params = params;
    func->type = type;
    func->return_value = NULL;
    func->has_definition = has_definition;
    func->is_extern = false;
    func->is_variadic = false;
    
    if (func->has_definition) {
        // if we are making a function, this means that we are currently in the global
        // scope hence no parent block
        func->block = cctx_new_block(cctx, NULL);
        cctx->current_block = func->block;
    } else {
        func->block = NULL;
    }
    
    cjvec_push(cctx->items, (void*) cctx_make_item(cctx, CItemFunction, (void*)func));
    return func;
}

void cctx_end_function(CContext* cctx, CFunction* func, CExpr* value) {
    UNUSED(cctx);
    func->return_value = value;
}

CType* cctx_create_type_int32(CContext* cctx, CIntSign sign, bool const_) {
    CType* ty = CCTX_ALLOC(cctx, CType);
    ty->kind = CTypeInt;
    ty->const_ = const_;
    ty->int_ty.sign = sign;
    return ty;
}

CType* cctx_create_type_string(CContext* cctx, bool const_) {
    CType* ty = CCTX_ALLOC(cctx, CType);
    ty->kind = CTypeString;
    ty->const_ = const_;
    return ty;    
}

CExpr* cctx_create_value_int(CContext* cctx, CType* type, int64_t value) {
    CExpr* expr = CCTX_ALLOC(cctx, CExpr);
    expr->type = type;
    expr->kind = CExprInt;
    expr->int_expr = value;
    return expr;
}

CExpr* cctx_create_value_string(CContext* cctx, CType* type, const char* value) {
    CExpr* expr = CCTX_ALLOC(cctx, CExpr);
    expr->type = type;
    expr->kind = CExprString;
    expr->string_expr = value;
    return expr;
}

CExpr* cctx_create_value_identifer(CContext* cctx, const char* identifier) {
    CExpr* expr = CCTX_ALLOC(cctx, CExpr);
    expr->kind = CExprIdentifier;
    expr->string_expr = identifier;
    return expr;
}

CExpr* cctx_add_expr(CContext* cctx, CExpr* lhs, CExpr* rhs) {
    CExpr* expr = CCTX_ALLOC(cctx, CExpr);
    expr->kind  = CExprBinaryOp;
    expr->binop.lhs = lhs;
    expr->binop.rhs = rhs;
    expr->binop.op = CBinaryAdd;
    return expr;
}

CExpr* cctx_sub_expr(CContext* cctx, CExpr* lhs, CExpr* rhs) {
    CExpr* expr = CCTX_ALLOC(cctx, CExpr);
    expr->kind  = CExprBinaryOp;
    expr->binop.lhs = lhs;
    expr->binop.rhs = rhs;
    expr->binop.op = CBinarySub;
    return expr;
}

void cctx_add_stmt(CContext* cctx, CStmt* stmt) {
    if (cctx->current_block) cjvec_push(cctx->current_block->statements, (void*)stmt);
    else UNREACHABLE;
}

void cctx_assign_value(CContext* cctx, CType* type, const char* name, CExpr* expr) {
    CStmt* stmt = CCTX_ALLOC(cctx, CStmt);
    stmt->kind = CStmtAssignVar;
    stmt->assign.name = name;
    stmt->assign.expr = expr;
    stmt->assign.type = type;

    cctx_add_stmt(cctx, stmt);
}

const char* cctx_type_to_str(CContext* cctx, CType* type) {
    JBuffer* tmp = jb_create();
    if (type->kind == CTypeInt) {
        CIntType int_type = type->int_ty;
        if (int_type.sign == CIntUnsigned) {
            jb_appendf_a(tmp, cctx->arena, "unsigned int");
        } else {
            jb_appendf_a(tmp, cctx->arena, "int");
        }
    } else if (type->kind == CTypeString) {
        if (type->const_) jb_appendf_a(tmp, cctx->arena,"const ");
        jb_appendf_a(tmp, cctx->arena, "char*");
    }
    const char* type_str = jb_str_a(tmp, cctx->arena);
    jb_free(tmp);
    return type_str;
}

const char* cctx_walk_expr(CContext* cctx, CExpr* expr) {
    switch (expr->kind) {
    case CExprInt: {
        char buf[32] = {0};
        sprintf(buf, "%ld", expr->int_expr);
        return jarena_strdup(cctx->arena, buf);
    }
    case CExprIdentifier:
        return expr->string_expr;
    case CExprString: {
        JBuffer* jb = jb_create();
        jb_appendf_a(jb, cctx->arena, "\"%s\"", expr->string_expr);
        const char* string = jb_str_a(jb, cctx->arena);
        jb_free(jb);
        return string;
    }
    case CExprBinaryOp: {
        JBuffer* jb = jb_create();

        switch(expr->binop.op) {
        case CBinaryAdd: jb_appendf_a(jb, cctx->arena, "%s + %s", cctx_walk_expr(cctx, expr->binop.lhs), cctx_walk_expr(cctx, expr->binop.rhs)); break;
        case CBinarySub: jb_appendf_a(jb, cctx->arena, "%s - %s", cctx_walk_expr(cctx, expr->binop.lhs), cctx_walk_expr(cctx, expr->binop.rhs)); break;
        default:
            TODO("implement all operators");
        }
        
        const char* result = jb_str_a(jb, cctx->arena);
        jb_free(jb);
        return result;
    }        
    default:
        TODO("implement other cexprs");
    }
    return NULL;
}

void cctx_walk_stmt(CContext* cctx, CStmt* stmt) {
    if (stmt->kind == CStmtAssignVar) {
        const char* name = stmt->assign.name;
        const char* type_str = cctx_type_to_str(cctx, stmt->assign.type);
        const char* expr_str = cctx_walk_expr(cctx, stmt->assign.expr);
        jb_appendf_a(cctx->body, cctx->arena, "%s%s %s = %s;\n", jtab_to_str(&cctx->tab_tracker), type_str, name, expr_str);
    }
}

void cctx_walk_block(CContext* cctx, CBlock* block) {
    jtab_add_level(&cctx->tab_tracker);
    FOREACH(CStmt*, stmt, i, block->statements) {
        cctx_walk_stmt(cctx, stmt);
    }
    jtab_sub_level(&cctx->tab_tracker);
}

const char* cctx_walk_parameters(CContext* cctx, CJVec* params) {
    JBuffer* tmp = jb_create();

    FOREACH(CParam*, param, i, params) {
        const char* name = param->name;
        const char* type = cctx_type_to_str(cctx, param->type);
        jb_appendf_a(tmp, cctx->arena, "%s %s", type, name);
        if (i == cjvec_len(params) - 1) break;
        jb_appendf_a(tmp, cctx->arena, ",");
    }
    const char* res = jb_str_a(tmp, cctx->arena);
    return res;
}

void cctx_walk_function(CContext* cctx, CFunction* func) {
    const char* name = func->name;
    const char* type_str = cctx_type_to_str(cctx, func->type);

    jb_appendf_a(cctx->body, cctx->arena, "%s %s %s (%s%s", (func->is_extern) ? "extern" : "", type_str, name,
    cctx_walk_parameters(cctx, func->params), func->is_variadic ? ", ...)" : ")");
    if (func->return_value) {
        TODO("implement return values");
    }

    if (func->has_definition) {
        // generate the body
        jb_appendf_a(cctx->body, cctx->arena, "\n{\n");
        if (func->block) {
            cctx_walk_block(cctx, func->block);
        }
        jb_appendf_a(cctx->body, cctx->arena, "}");
    } else {
        jb_appendf_a(cctx->body, cctx->arena, ";");        
    }
    jb_appendf_a(cctx->body, cctx->arena, " // %s\n", name);
}

void cctx_walk_items(CContext* cctx) {
    FOREACH(CItem*, item, i, cctx->items) {
        if (item->kind == CItemFunction) {
            cctx_walk_function(cctx, (CFunction*)item->data);
        }
    }
}


JBuffer* cctx_get_output(CContext* cctx) {
    if (!cctx->output_built) {
        cctx->final_output = jb_create();
        cctx_walk_items(cctx);
        
        const char* includes = jb_len(cctx->includes) == 0 ? NULL : jb_str_a(cctx->includes, cctx->arena);
        const char* header   = jb_len(cctx->header)   == 0 ? NULL : jb_str_a(cctx->header, cctx->arena);
        const char* body     = jb_len(cctx->body)     == 0 ? NULL : jb_str_a(cctx->body, cctx->arena);
        
        if (includes) jb_appendf_a(cctx->final_output, cctx->arena, "%s\n", includes);
        if (header) jb_appendf_a(cctx->final_output, cctx->arena, "%s\n", header);
        if (body) jb_appendf_a(cctx->final_output, cctx->arena, "%s", body);
        cctx->output_built = true;
    }    
    
    return cctx->final_output;        
}

void cctx_free(CContext* cctx) {
    jb_free(cctx->includes);
    jb_free(cctx->header);
    jb_free(cctx->body);
    
    if (cctx->output_built) jb_free(cctx->final_output);
}
