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

  TOKEN_LET,

  TOKEN_FN,

  TOKEN_IF,
  TOKEN_THEN,
  TOKEN_ELSE,
} TokenType;

typedef struct {
    TokenType tt;

    u32 start, len, line, location;
} Token;

TokenType Meridian_GetKeyword(String src, Token t);

#endif//MERIDIAN_TOKENS_H
