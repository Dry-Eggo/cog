#pragma once

#include <stdlib.h>

#define LOG_ERR(msg, ...) fprintf(stderr, "Kudo Error: "msg, ##__VA_ARGS__)
#define c(k) get_color(&global_color_manager, k)
#define ALLOC(ty) (ty*)jarena_alloc(global_arena, sizeof(ty))
#define UNUSED(var) (void)var

#define TODO(msg, ...) do {\
fprintf(stderr, "Unimplemented %s:%d: "msg"\n", __FILE__, __LINE__, ##__VA_ARGS__);\
exit(1);\
} while (0)

#define UNREACHABLE do {\
fprintf(stderr, "Unreachable %s:%d\n", __FILE__, __LINE__);\
exit(1);\
} while (0)

#define ASSERT(cond, msg, ...) do {\
if (!(cond)) {\
fprintf(stderr, "Assertion failed: (%s) %s:%d: "msg"\n", #cond, __FILE__, __LINE__,  ##__VA_ARGS__);\
exit(1);\
} }while (0)

#define FOREACH(ty, val, i, l) \
for (size_t i = 0; i < cjvec_len(l); ++i)\
for (int _once = 1; _once; _once = 0)\
for (ty val = cjvec_at(l, i); _once; _once = 0)
