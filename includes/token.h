#pragma once

enum TokenKind {
  T_LPAREN,
  T_RPAREN,
  T_KEYWORD,
  T_NUMBER,
  T_IDENT,
  T_STRING,
  T_EOL
};
char *TK_tostr(enum TokenKind k);
struct Token {
  enum TokenKind kind;
  char *lexme;
};
struct TokenList {
  int count;
  int cap;
  struct Token *tokens;
};
void TL_init(struct TokenList **tl);
void TL_grow(struct TokenList *tl);
void TL_add(struct TokenList *tl, struct Token t);
void TL_dump(struct TokenList *tl);
int iskeyword(char *key);
struct TokenList *lex(char *line);
struct Token create_token(enum TokenKind kind, char *lexme);
