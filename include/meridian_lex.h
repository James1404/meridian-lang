#ifndef MERIDIAN_LEX_H
#define MERIDIAN_LEX_H

#include "meridian_string.h"
#include "meridian_tokens.h"

typedef struct {
    String src;

    u64 current, line, lineOffset;

    Token* tokens;
    u64 tokensLen, tokensAllocated;
} Lexer;

Lexer Lexer_new(String src);
void Lexer_free(Lexer *lexer);

void Lexer_run(Lexer *lexer);

void Lexer_print(Lexer* lexer);

#endif//MERIDIAN_LEX_H
