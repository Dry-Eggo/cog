#pragma once
#include "builtins.h"


struct Program {
  struct Closure **closures;
  int closure_count;
};

struct Program *create_program(struct TokenList *tl);
struct Closure *create_closure(const char *name, struct Value **args,
                               int arg_count);
