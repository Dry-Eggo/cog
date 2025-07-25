#pragma once

#define log_err(msg, ...) fprintf(stderr, "Kudo Error: "msg, ##__VA_ARGS__)
#define c(k) get_color(&global_color_manager, k)
