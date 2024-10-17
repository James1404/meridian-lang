#ifndef MERIDIAN_PARSER_H
#define MERIDIAN_PARSER_H

#include "meridian_ast.h"
#include "meridian_tokens.h"

typedef struct {
    u64 current;

    NodeList* tree;
    NodeIndex root;

    TokenList* tokens;
} Parser;

Parser Parser_make(TokenList* in, NodeList* out);
void Parser_free(Parser *parser);

void Parser_run(Parser* parser);

NodeIndex Parser_Ident(Parser *parser);
NodeIndex Parser_Value(Parser *parser);
NodeIndex Parser_Type(Parser *parser);
NodeIndex Parser_Expression(Parser *parser);
NodeIndex Parser_Stmt(Parser *parser);
NodeIndex Parser_Define(Parser *parser);
NodeIndex Parser_TypeDef(Parser *parser);
NodeIndex Parser_Let(Parser *parser);
NodeIndex Parser_If(Parser *parser);

NodeIndex Parser_Abstraction(Parser *parser);

NodeIndex Parser_Scope(Parser *parser);
NodeIndex Parser_TopLevelScope(Parser *parser);

#endif//MERIDIAN_PARSER_H
