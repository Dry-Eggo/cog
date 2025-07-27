#pragma once

#include <unistd.h>
#include <stdio.h>
#include <enum.h>
#include <stdbool.h>

typedef struct {
    bool use_color;
    const char* colors[color_count_k];
} ColorManager;


void cm_init(ColorManager* cm);
const char* get_color(ColorManager* cm, color_t c);
