#ifndef MERIDIAN_LEX_H
#define MERIDIAN_LEX_H

#include "meridian_string.h"
#include "meridian_tokens.h"

typedef struct {
    String src;

    u64 current, start, line, lineOffset;

    TokenList* tokens;
} Lexer;

Lexer Lexer_new(TokenList* out);
void Lexer_free(Lexer *lexer);

void Lexer_run(Lexer *lexer);

void Lexer_print(Lexer* lexer);

#endif//MERIDIAN_LEX_H
