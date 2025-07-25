#pragma once

#include <stdlib.h>

#define log_err(msg, ...) fprintf(stderr, "Kudo Error: "msg, ##__VA_ARGS__)
#define c(k) get_color(&global_color_manager, k)
#define alloc(ty) (ty*)jarena_alloc(global_arena, sizeof(ty))


#define todo(msg, ...) do {\
fprintf(stderr, "Unimplemented %s:%d: "msg"\n", __FILE__, __LINE__, ##__VA_ARGS__);\
exit(1);\
} while (0)
