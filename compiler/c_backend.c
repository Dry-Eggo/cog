#include <c_backend.h>
#include <compiler.h>
#include <defines.h>

typedef struct CInt CInt;
struct CInt {
    CIntSign sign;
    double value;  
};

struct CContext {
    JBuffer* includes; // includes
    JBuffer* header;   // foward_decl    
    JBuffer* body;     // main code

    CJVec*   functions;
    JArena*  arena;
};

struct CFunction {
    const char* name;
    CType* type;    
    CJVec* sattr; // extern, inline, static
    CJVec* params;
};

struct CType {
    CTypeKind kind;
};

CContext* cctx_new(JArena* arena) {
    CContext* ctx = alloc(CContext);
    ctx->includes = jb_create();
    ctx->header   = jb_create();
    ctx->body     = jb_create();

    ctx->functions = cjvec_new(global_arena);

    ctx->arena = arena;
    return ctx;
}
