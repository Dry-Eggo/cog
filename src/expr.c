
#include "../includes/expr.h"
#include "../includes/tree.h"
#include <stdio.h>
#include <stdlib.h>
struct Value *eval_expr(struct Scope *sc, struct Closure *expr) {
  if (strcmp(expr->callee, "set") == 0) {
    if (expr->arg_count != 2) {
      printf("Param Count MisMatch: 'set' expected 2 arguments\n");
      return NULL;
    }
    return builtin_set(sc, expr);
  } else if (strcmp(expr->callee, "scope") == 0) {
    return builtin_scopedump(sc, expr);
  } else {
    printf("Undecalred closure or Symbol: '%s'\n", expr->callee);
    return NULL;
  }
}

struct Value *eval_line(struct Scope *sc, char *line) {
  struct TokenList *t = lex(line);
  struct Program *program = create_program(t);
  free(t);
  if (program && program->closures)
    return eval_expr(sc, program->closures[0]);
  return NULL;
}
