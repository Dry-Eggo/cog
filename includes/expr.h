#pragma once
#include "builtins.h"
#include "tree.h"

enum ExprKind {
  ExprInt = 0,
  ExprFuncCall,
};

struct Expr {
  enum ExprKind kind;
  struct Value value;
};
struct Value *eval_line(struct Scope *sc, char *line);
struct Value *eval_expr(struct Scope *sc, struct Closure *);
