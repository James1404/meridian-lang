#include "meridian_parser.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include "meridian_tokens.h"

Parser Parser_make(TokenList *in, ASTList* out) {
    return (Parser) {
        .current = 0,
        .tree = out,

        .tokens = in,
    };
}

void Parser_free(Parser *parser) {}

static bool Parser_EOF(Parser* parser) {
    return parser->current >= parser->tokens->len;
}

static void Parser_adv(Parser *parser) {
    parser->current++;
}

static Token Parser_current(Parser* parser) {
    return parser->tokens->data[parser->current];
}

static bool Parser_match(Parser* parser, TokenType expected) {
    if(Parser_current(parser).tt == expected) {
        Parser_adv(parser);
        return true;
    }

    return false;
}

AST_Idx Parser_Ident(Parser *parser) {
    Token t = Parser_current(parser);

    if(!Parser_match(parser, TOKEN_IDENT)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }
    
    AST_Idx id = AST_MAKE(parser->tree, AST_IDENT);
    String text = { parser->tokens->src.raw + t.start, t.len };
    AST_VALUE(parser->tree, id, AST_IDENT) = text;

    return id;
}

AST_Idx Parser_Value(Parser *parser) {
    switch(Parser_current(parser).tt) {
    case TOKEN_IDENT: {
        return Parser_Ident(parser);
    }
        
    case TOKEN_INT: {
        String text = TOKENLIST_text(parser->tokens, Parser_current(parser));

        String cpy;
        STR_CPY_ALLOC_NULL(cpy, text);

        AST_Idx result = AST_MAKE_V(parser->tree, AST_INTEGER, strtoll(cpy.raw, NULL, 10));

        STR_FREE(cpy);

        Parser_adv(parser);
        return result;
    }
    case TOKEN_FLOAT: {
        String text = TOKENLIST_text(parser->tokens, Parser_current(parser));

        String cpy;
        STR_CPY_ALLOC_NULL(cpy, text);

        AST_Idx result = AST_MAKE_V(parser->tree, AST_FLOAT, strtof(cpy.raw, NULL));

        STR_FREE(cpy);

        Parser_adv(parser);
        return result;
    }

    case TOKEN_TRUE: {
        Parser_adv(parser);
        return AST_MAKE_V(parser->tree, AST_BOOLEAN, true);
    }
    case TOKEN_FALSE: {
        Parser_adv(parser);
        return AST_MAKE_V(parser->tree, AST_BOOLEAN, false);
    }

    case TOKEN_STRING: {
        AST_Idx result = AST_MAKE_V(parser->tree, AST_STRING, TOKENLIST_text(parser->tokens, Parser_current(parser)));
        Parser_adv(parser);
        return result;
    }

    case TOKEN_LET: {
        return Parser_Let(parser);
    }
    case TOKEN_IN: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'in'");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    case TOKEN_IF: {
        return Parser_If(parser);
    }
    case TOKEN_THEN: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'then'");
        return AST_MAKE(parser->tree, AST_NULL);
    }
    case TOKEN_ELSE: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'else'");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    case TOKEN_LPAREN: {
        Parser_adv(parser);
        AST_Idx result = Parser_Expression(parser);
        if(!Parser_match(parser, TOKEN_RPAREN)) {
            Meridian_error("Expected closing ')'");
            return AST_MAKE(parser->tree, AST_NULL);
        }
        
        return result;
    }
    case TOKEN_RPAREN: {
        Parser_adv(parser);
        Meridian_error("Unexpected ')'");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    case TOKEN_FN: {
        return Parser_Abstraction(parser);
    }
        
    default: {
        return AST_MAKE(parser->tree, AST_NULL);
    }
    }
}

AST_Idx Parser_Let(Parser *parser) {
    if(!Parser_match(parser, TOKEN_LET)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx name = Parser_Ident(parser);

    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in let");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx value = Parser_Expression(parser);

    if(!Parser_match(parser, TOKEN_IN)) {
        Meridian_error("Expected 'in' in let");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx in = Parser_Expression(parser);

    return AST_MAKE_S(parser->tree, AST_LET, name, value, in);
}

AST_Idx Parser_If(Parser *parser) {
    if(!Parser_match(parser, TOKEN_IF)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx cond = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_THEN)) {
        Meridian_error("Expected 'then' in if expression");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx t = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_ELSE)) {
        Meridian_error("Expected 'else' in if expression");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx f = Parser_Expression(parser);

    return AST_MAKE_S(parser->tree, AST_IF, cond, t, f);
}

AST_Idx Parser_Type(Parser *parser) {
    switch(Parser_current(parser).tt) {
    case TOKEN_IDENT: return Parser_Ident(parser);
    default:
        Parser_adv(parser);
        return AST_MAKE(parser->tree, AST_NULL);
    }
}

AST_Idx Parser_Expression(Parser* parser) {
    AST_Idx first = Parser_Value(parser);

    if(Parser_match(parser, TOKEN_ANNOTATE)) {
        AST_Idx type = Parser_Type(parser);
        return AST_MAKE_S(parser->tree, AST_ANNOTATE, first, type);
    }

    AST_Idx arg = Parser_Value(parser);

    if(AST_TY(parser->tree, arg) != AST_NULL) {
        return AST_MAKE_S(parser->tree, AST_APPLICATION, first, arg);
    }

    return first;
}

AST_Idx Parser_Abstraction(Parser* parser) {
    if(!Parser_match(parser, TOKEN_FN)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx arg = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ARROW)) {
        Meridian_error("Expected '->' in fn abstraction");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx body = Parser_Expression(parser);

    return AST_MAKE_S(parser->tree, AST_ABSTRACTION, arg, body);
}

AST_Idx Parser_Define(Parser *parser) {
    if(!Parser_match(parser, TOKEN_DEF)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx body = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_SEMICOLON)) {
        Meridian_error("Expected ';' at end of definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    return AST_MAKE_S(parser->tree, AST_DEFINE, name, body);
}

AST_Idx Parser_TypeDef(Parser *parser) {
    if(!Parser_match(parser, TOKEN_TYPE)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in type definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_Idx type = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_SEMICOLON)) {
        Meridian_error("Expected ';' at end of type definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    return AST_MAKE_S(parser->tree, AST_TYPEDEF, name, type);  
}

AST_Idx Parser_Stmt(Parser *parser) {
    switch(Parser_current(parser).tt) {
    case TOKEN_DEF:
        return Parser_Define(parser);
    case TOKEN_TYPE:
        return Parser_TypeDef(parser);
    default:
        return AST_MAKE(parser->tree, AST_NULL);
    }
}

AST_Idx Parser_TopLevelScope(Parser* parser) {
    AST_Idx root = AST_MAKE_S(parser->tree, AST_SCOPE, 0);

    AST_Idx current = AST_VALUE(parser->tree, root, AST_SCOPE).start = AST_MAKE_S(parser->tree, AST_CONS, 0, 0);
    AST_Idx prev = current;

    do {
        AST_Idx data = Parser_Stmt(parser);
        AST_VALUE(parser->tree, current, AST_CONS).data = data;

        AST_Idx next = AST_MAKE_S(parser->tree, AST_CONS, 0, 0);
        AST_VALUE(parser->tree, current, AST_CONS).next = next;
        prev = current;
        current = next;
    } while(!Parser_EOF(parser));

    AST_VALUE(parser->tree, prev, AST_CONS).next = AST_MAKE(parser->tree, AST_NULL);
    
    return root;
}

void Parser_run(Parser *parser) {
    parser->root = Parser_TopLevelScope(parser);
}
