#include "../includes/builtins.h"
#include <stdint.h>

struct Value *Value_from_tk(struct Token t) {
  struct Value *val = (struct Value *)malloc(sizeof(struct Value));
  if (t.kind == T_NUMBER) {
    val->kind = ValueInt;
    val->int_data = (int)strtol(t.lexme, NULL, 10);
    return val;
  } else if (t.kind == T_STRING) {
    char buf[64] = {0};
    int i = snprintf(buf, sizeof(buf), "'%s'", t.lexme);
    buf[i] = '\0';
    val->kind = ValueString;
    val->string_data = strdup(buf);
    return val;
  } else if (t.kind == T_IDENT) {
    val->kind = ValueObject;
    val->identifier = t.lexme;
    return val;
  }
  printf("Invalid Value '%s'\n", t.lexme);
  exit(1);
}
struct Variable *create_variable(const char *name, struct Value *value) {
  struct Variable *v = (struct Variable *)malloc(sizeof(struct Variable));
  v->name = name;
  v->value = value;
  return v;
}
void SC_init(struct Scope **sc) {
  *sc = (struct Scope *)malloc(sizeof(struct Scope));
  (*sc)->count = 0;
  (*sc)->cap = SC_INIT_CAP;
  (*sc)->variables =
      (struct Variable **)malloc(sizeof(struct Variable) * (*sc)->cap);
}
const char *Value_tostr(struct Value *v) {
  if (!v)
    return "NULL";
  switch (v->kind) {
  case ValueString:
    return v->string_data;
  case ValueInt: {
    char buf[64] = {0};
    sprintf(buf, "%d", v->int_data);
    return strdup(buf);
  }
  case ValueObject: {
    return v->identifier;
  }
  default:
    return "NULL";
  }
}
void SC_grow(struct Scope *sc) {
  sc->variables = (struct Variable **)realloc(
      sc->variables, sizeof(struct Variable) * sc->cap * 2);
  sc->cap *= 2;
}
void SC_add(struct Scope *sc, struct Variable *t) {
  if (sc->count >= sc->cap) {
    SC_grow(sc);
  }
  sc->variables[sc->count++] = t;
}
int SC_find(struct Scope *sc, const char *name) {
  int i = 0;
  int max = sc->count;
  while (i < max) {
    struct Variable *v = sc->variables[i];
    if (strcmp(v->name, name) == 0) {
      return 1;
    }
  }
  return 0;
}
struct Variable *SC_get(struct Scope *sc, char *name) {

  int i = 0;
  int max = sc->count;
  while (i < max) {
    struct Variable *v = sc->variables[i];
    if (strcmp(v->name, name) == 0) {
      return v;
    }
  }
  return NULL;
}
void SC_dump(struct Scope *sc) {
  int c = 0;
  int smax = sc->count;
  while (c < smax) {
    struct Variable *v = sc->variables[c];
    printf("{%s : %s}\n", v->name, Value_tostr(v->value));
    ++c;
  }
}

/* runtime builtins */
struct Value *builtin_set(struct Scope *sc, struct Closure *c) {
  if (c->arg_count > 1) {
    const char *identifier = c->args[0]->identifier;
    if (SC_find(sc, identifier)) {
      printf("Redefinition of '%s'. use (re %s val) to mutate\n", identifier,
             identifier);
      return NULL;
    }
    struct Variable *val = create_variable(identifier, c->args[1]);
    SC_add(sc, val);
    return val->value;
  }
  return NULL;
}

struct Value *builtin_scopedump(struct Scope *sc, struct Closure *c) {
  int max = sc->count;
  printf("Variables:\n");
  for (int i = 0; i < max; ++i) {
    struct Variable *var = sc->variables[i];
    printf("    %s : %s\n", var->name, Value_tostr(var->value));
  }
  return NULL;
}
