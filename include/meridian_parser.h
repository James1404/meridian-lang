#ifndef MERIDIAN_PARSER_H
#define MERIDIAN_PARSER_H

#include "meridian_ast.h"
#include "meridian_tokens.h"

typedef struct {
    u64 current;

    Ast* tree;

    TokenList tokens;
} Parser;

Parser Parser_make(void);


#endif//MERIDIAN_PARSER_H
