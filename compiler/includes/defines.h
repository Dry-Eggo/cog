#pragma once

#define log_err(msg, ...) fprintf(stderr, "Kudo Error: "msg, ##__VA_ARGS__)
