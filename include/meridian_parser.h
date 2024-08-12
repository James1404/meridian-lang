#ifndef MERIDIAN_PARSER_H
#define MERIDIAN_PARSER_H

#include "meridian_ast.h"
#include "meridian_tokens.h"

typedef struct {
    u64 current;

    ASTList* tree;
    AST_Idx root;

    TokenList* tokens;
} Parser;

Parser Parser_make(TokenList* in, ASTList* out);
void Parser_free(Parser *parser);

void Parser_run(Parser* parser);

AST_Idx Parser_Ident(Parser *parser);
AST_Idx Parser_Value(Parser *parser);
AST_Idx Parser_Type(Parser *parser);
AST_Idx Parser_Expression(Parser *parser);
AST_Idx Parser_Stmt(Parser *parser);
AST_Idx Parser_Define(Parser *parser);
AST_Idx Parser_TypeDef(Parser *parser);
AST_Idx Parser_Let(Parser *parser);
AST_Idx Parser_If(Parser *parser);

AST_Idx Parser_Abstraction(Parser *parser);

AST_Idx Parser_Scope(Parser *parser);
AST_Idx Parser_TopLevelScope(Parser *parser);

#endif//MERIDIAN_PARSER_H
