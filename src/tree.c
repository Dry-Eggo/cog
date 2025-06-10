#include "../includes/tree.h"
#include <stdlib.h>

struct Value *parse_value(struct TokenList *tl, int *index) {
  return Value_from_tk(tl->tokens[(*index)++]);
}
#define ARG_MAX 10
void Args_init(struct Value ***args) {
  *args = (struct Value **)malloc(sizeof(struct Value *) * ARG_MAX);
}

struct Closure *create_closure(const char *i, struct Value **args, int argc) {
  struct Closure *c = (struct Closure *)malloc(sizeof(struct Closure));
  c->callee = i;
  c->args = args;
  c->arg_count = argc;
  return c;
}

#define PROG_MAX 1024
struct Program *create_program(struct TokenList *tl) {
  int index = 0;
  int max = tl->count;
  struct Program *program = (struct Program *)malloc(sizeof(struct Program));
  program->closures =
      (struct Closure **)malloc(sizeof(struct Closure *) * PROG_MAX);
  while (index < max) {
    struct Token tok = tl->tokens[index];
    if (tok.kind == T_LPAREN) {
      ++index;
      if (tl->tokens[index].kind == T_KEYWORD || T_IDENT) {
        const char *identifier = tl->tokens[index].lexme;
        ++index;
        struct Value **args;
        Args_init(&args);
        int argc = 0;
        while ((index < max) && tl->tokens[index].kind != T_RPAREN) {
          struct Value *arg = parse_value(tl, &index);
          args[argc] = arg;
          argc++;
        }
        program->closures[program->closure_count++] =
            create_closure(identifier, args, argc);
      }
      if (tl->tokens[index].kind != T_RPAREN) {
        printf("Unterminated '('\n");
        return NULL;
      }
    } else {
      printf("Invalid Token\n");
      return NULL;
    }

    ++index;
  }
  return program;
}
