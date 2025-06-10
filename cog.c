#include "includes/builtins.h"
#include "includes/expr.h"
#include <stdio.h>
#include <string.h>

int main(int c, char *v[]) {
  struct Scope *sc;
  SC_init(&sc);
  while (1) {
    printf("cog> ");
    char buffer[1024] = {0};
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "#exit") == 0) {
      break;
    }
    struct Value *v = eval_line(sc, buffer);
    if (v)
      printf("=> %s\n", Value_tostr(v));
    free(v);
  }
}
