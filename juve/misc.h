#pragma once
#include "juve_utils.h"

typedef struct {
    int  indent;
    juve_arena_t* arena;
} jtab_tracker_t;


jtab_tracker_t jtab_new(juve_arena_t* arena);

void jtab_add_level(jtab_tracker_t* tracker);
void jtab_sub_level(jtab_tracker_t* tracker);

const char* jtab_to_str(jtab_tracker_t* tracker);
