#include <c_backend.h>
#include <compiler.h>
#include <defines.h>

#define CCTX_ALLOC(ctx, ty) (ty*)jarena_alloc((ctx)->arena, sizeof(ty))

typedef struct CIntType CIntType;
typedef struct CItem CItem;

typedef enum {
    CItemFunction,
    CItemStruct,
    CItemTypeDef
} CItemKind;

struct CItem {
    CItemKind kind;
    void* data;
};

struct CIntType {
    CIntSign sign;
};

struct CContext {
    JBuffer* includes; // includes
    JBuffer* header;   // foward_decl    
    JBuffer* body;     // main code

    JBuffer* final_output;
    
    CJVec*   items;
    JArena*  arena;
};

struct CFunction {
    const char* name;
    CType* type;    
    CJVec* sattr; // extern, inline, static
    CJVec* params;

    CValue* return_value;
    bool    ended;
};

struct CType {
    CTypeKind kind;
    bool      const_;

    union {
        CIntType int_ty;
    };
};


CContext* cctx_new(JArena* arena) {
    CContext* ctx = alloc(CContext);
    ctx->includes = jb_create();
    ctx->header   = jb_create();
    ctx->body     = jb_create();

    ctx->items = cjvec_new(global_arena);

    ctx->arena = arena;
    return ctx;
}

CItem* cctx_make_item(CContext* cctx, CItemKind kind, void* data) {
    CItem* item = CCTX_ALLOC(cctx, CItem);
    item->kind = kind;
    item->data = data;
    return item;
}

CFunction* cctx_create_function(CContext* cctx, const char* name, CJVec* params, CJVec* sattr, CType* type) {
    CFunction* func = CCTX_ALLOC(cctx, CFunction);
    func->name = name;
    func->params = params;
    func->sattr = sattr;
    func->type = type;
    func->return_value = NULL;
    cjvec_push(cctx->items, (void*) cctx_make_item(cctx, CItemFunction, (void*)func));
    return func;
}

void cctx_end_function(CContext* cctx, CFunction* func, CValue* value) {
    func->return_value = value;
    func->ended = true;
}

CType* cctx_create_type_int32(CContext* cctx, CIntSign sign, bool const_) {
    CType* ty = CCTX_ALLOC(cctx, CType);
    ty->kind = CTypeInt;
    ty->const_ = const_;
    ty->int_ty.sign = sign;
    return ty;
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
    }
    const char* type_str = jb_str_a(tmp, cctx->arena);
    jb_free(tmp);
    return type_str;
}

void cctx_walk_function(CContext* cctx, CFunction* func) {
    const char* name = func->name;
    const char* type_str = cctx_type_to_str(cctx, func->type);

    jb_appendf_a(cctx->body, cctx->arena, "%s %s()\n", type_str, name);
    if (func->return_value) {
        todo("implement return values");
    }

    if (func->ended) {
        // generate the body
        jb_appendf_a(cctx->body, cctx->arena, "{\n\n}");
    } else {
        jb_appendf_a(cctx->body, cctx->arena, ";");        
    }
    jb_appendf_a(cctx->body, cctx->arena, "// %s\n", name);
}

void cctx_walk_items(CContext* cctx) {
    fori(CItem*, item, i, cctx->items) {
        if (item->kind == CItemFunction) {
            cctx_walk_function(cctx, (CFunction*)item->data);
        }
    }
}


JBuffer* cctx_get_output(CContext* cctx) {
    cctx->final_output = jb_create();
    cctx_walk_items(cctx);

    const char* includes = jb_str_a(cctx->includes, cctx->arena);
    const char* header   = jb_str_a(cctx->header, cctx->arena);
    const char* body     = jb_str_a(cctx->body, cctx->arena);

    jb_appendf_a(cctx->final_output, cctx->arena, "%s\n%s\n%s\n", includes, header, body);
    
    return cctx->final_output;
}
