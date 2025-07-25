#pragma once

#include <unistd.h>
#include <stdio.h>
#include <enum.h>
#include <stdbool.h>

typedef struct {
    bool use_color;
    const char* colors[color_count_k];
} color_manager_t;


void cm_init(color_manager_t* cm);
const char* get_color(color_manager_t* cm, color_t c);
