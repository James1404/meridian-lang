#ifndef MERIDIAN_TOKENS_H
#define MERIDIAN_TOKENS_H

/*

  TOKEN_IDENTIFIER :=
  | 'A' .. 'Z' | 'a' ..'z' | '0' .. '9' | '_'
  | '+' | '-' | '/' | '*'
  | '<' | '>'
  

 */

#include "meridian_common.h"
#include "meridian_string.h"

typedef enum {
  TOKEN_NONE,

  TOKEN_INT,
  TOKEN_FLOAT,

  TOKEN_IDENT,

  TOKEN_STRING,

  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,

  TOKEN_TRUE,
  TOKEN_FALSE,

  TOKEN_ASSIGN,
  TOKEN_ARROW,
  TOKEN_ANNOTATE,

  TOKEN_SEMICOLON,
  TOKEN_COLON,

  TOKEN_LET, TOKEN_IN,

  TOKEN_MATCH,

  TOKEN_TYPE,
  TOKEN_DEF,

  TOKEN_FN,

  TOKEN_IF,
  TOKEN_THEN,
  TOKEN_ELSE,
} TokenType;

typedef struct {
    TokenType tt;

    u32 start, len, line, lineOffset;
} Token;

TokenType Meridian_GetKeyword(String src, Token t);

typedef struct {
    Token* data;
    u64 len, allocated;

    String src;
} TokenList;

TokenList TokenList_make(String src);
void TokenList_free(TokenList *list);

void TokenList_push(TokenList *list, Token elem);

#define TOKENLIST_text(list, token) ((String) { list->src.raw + token.start, token.len })

#endif//MERIDIAN_TOKENS_H
