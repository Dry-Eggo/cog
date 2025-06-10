#pragma once
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Closure {
  const char *callee;
  struct Value **args;
  int arg_count;
};
#define SC_INIT_CAP 1024
enum ValueKind {
  ValueInt = 0,
  ValueString,
  ValueChar,
  ValueFloat,
  ValueObject,
};

struct Value {
  enum ValueKind kind;
  union {
    const char *string_data;
    int int_data;
    float float_data;
    char char_data;
    const char *identifier;
  };
};

struct Value *Value_from_tk(struct Token t);
const char *Value_tostr(struct Value *v);
struct Variable {
  const char *name;
  struct Value *value;
};
struct Variable *create_variable(const char *name, struct Value *value);
struct Scope {
  int count;
  int cap;
  struct Variable **variables;
};

void SC_grow(struct Scope *sc);
void SC_init(struct Scope **sc);
void SC_add(struct Scope *sc, struct Variable *t);
void SC_dump(struct Scope *sc);
int SC_find(struct Scope *sc, const char *name);

struct Variable *SC_get(struct Scope *sc, char *name);

/* runtime builtins */
struct Value *builtin_set(struct Scope *sc, struct Closure *c);
struct Value* builtin_scopedump(struct Scope* sc, struct Closure*);
