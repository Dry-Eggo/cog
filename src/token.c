#include "../includes/token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *TK_tostr(enum TokenKind k) {
  switch (k) {
  case T_LPAREN:
    return "LParen";
  case T_RPAREN:
    return "RParen";
  case T_IDENT:
    return "Identifier";
  case T_KEYWORD:
    return "Keyword";
  case T_NUMBER:
    return "Integer";
  default:
    return "<unimplemented>";
  }
}
#define TL_INIT_CAP 1024
void TL_init(struct TokenList **tl) {
  *tl = (struct TokenList *)malloc(sizeof(struct TokenList));
  (*tl)->count = 0;
  (*tl)->cap = TL_INIT_CAP;
  (*tl)->tokens = (struct Token *)malloc(sizeof(struct Token) * (*tl)->cap);
}
void TL_grow(struct TokenList *tl) {
  tl->tokens = realloc(tl->tokens, tl->cap * 2);
  tl->cap *= 2;
}
void TL_add(struct TokenList *tl, struct Token t) {
  if (tl->count >= tl->cap) {
    TL_grow(tl);
  }
  tl->tokens[tl->count++] = t;
}
void TL_dump(struct TokenList *tl) {
  printf("Tokens:\n");
  for (int i = 0; i < tl->count; ++i) {
    struct Token t = tl->tokens[i];
    printf("    %s ('%s') \n", TK_tostr(t.kind), t.lexme);
  }
}
int iskeyword(char *key) {
  if (strcmp(key, "set") == 0) {
    return 1;
  }
  if (strcmp(key, "dump") == 0) {
    return 1;
  }
  if (strcmp(key, "re") == 0) {
    return 1;
  }
  if (strcmp(key, "scope") == 0) {
    return 1;
  }
  if (strcmp(key, "add") == 0) {
    return 1;
  }
  return 0;
}
struct TokenList *lex(char *line) {
  int i = 0;
  int max = strlen(line);
  struct TokenList *tokens;
  TL_init(&tokens);
  while (i < max) {
    if (isspace(line[i])) {
      while (isspace(line[i])) {
        i++;
      }
    }
    if (isalpha(line[i]) || line[i] == '_') {
      char buf[64] = {0};
      int inx = 0;
      while ((i < max) && (isalnum(line[i]) || line[i] == '_')) {
        buf[inx++] = line[i++];
      }
      buf[inx] = '\0';
      if (iskeyword(buf)) {
        TL_add(tokens, create_token(T_KEYWORD, strdup(buf)));
        continue;
      }
      TL_add(tokens, create_token(T_IDENT, strdup(buf)));
      continue;
    }
    if (isdigit(line[i])) {
      char buf[64] = {0};
      int inx = 0;
      while ((i < max) && isdigit(line[i])) {
        buf[inx++] = line[i++];
      }
      buf[inx] = '\0';
      TL_add(tokens, create_token(T_NUMBER, strdup(buf)));
      continue;
    }
    if (line[i] == '\"') {
      ++i;
      char buf[64] = {0};
      int inx = 0;
      while (line[i] != '\"') {
        buf[inx++] = line[i++];
      }
      if (line[i] != '\"') {
        printf("Unterminated String Sequence\n");
        return NULL;
      }
      ++i;
      buf[inx] = '\0';
      TL_add(tokens, create_token(T_STRING, strdup(buf)));
      continue;
    }
    switch (line[i]) {
    case '(':
      i++;
      TL_add(tokens, create_token(T_LPAREN, "("));
      break;
    case ')':
      i++;
      TL_add(tokens, create_token(T_RPAREN, ")"));
      break;
    default:
      printf("Error: Unexpected Character '%c'\n", line[i]);
      exit(1);
    }
  }
  return tokens;
}
struct Token create_token(enum TokenKind kind, char *lexme) {
  return (struct Token){
      kind,
      lexme,
  };
}
