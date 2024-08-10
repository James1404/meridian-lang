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
        AST_Idx result = AST_MAKE_V(parser->tree, AST_INTEGER, 25);
        Parser_adv(parser);
        return result;
    }
    case TOKEN_FLOAT: {
        AST_Idx result = AST_MAKE_V(parser->tree, AST_FLOAT, 25.0f);
        Parser_adv(parser);
        return result;
    }

    case TOKEN_STRING: {
        AST_Idx result = AST_MAKE_V(parser->tree, AST_STRING, TOKENLIST_text(parser->tokens, Parser_current(parser)));
        Parser_adv(parser);
        return result;
    }

    case TOKEN_FN: {
        return Parser_Abstraction(parser);
    }
        
    default:
        return AST_MAKE(parser->tree, AST_NULL);
    }
}

AST_Idx Parser_Expression(Parser* parser) {
    return Parser_Value(parser);
}

AST_Idx Parser_Abstraction(Parser* parser) {
    AST_Idx result = AST_MAKE(parser->tree, AST_ABSTRACTION);
    
    if(!Parser_match(parser, TOKEN_FN)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_ABSTRACTION).arg = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ARROW)) {
        Meridian_error("Expected '->' in fn abstraction");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_ABSTRACTION).body = Parser_Value(parser);

    return result;
}

AST_Idx Parser_Define(Parser *parser) {
    AST_Idx result = AST_MAKE(parser->tree, AST_DEFINE);
    
    if(!Parser_match(parser, TOKEN_DEF)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_DEFINE).name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_DEFINE).body = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_SEMICOLON)) {
        Meridian_error("Expected ';' at end of definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    return result;
}

AST_Idx Parser_TypeDef(Parser *parser) {
    AST_Idx result = AST_MAKE(parser->tree, AST_TYPEDEF);
    
    if(!Parser_match(parser, TOKEN_TYPE)) {
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_TYPEDEF).name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in type definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    AST_VALUE(parser->tree, result, AST_TYPEDEF).type = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_SEMICOLON)) {
        Meridian_error("Expected ';' at end of type definition");
        return AST_MAKE(parser->tree, AST_NULL);
    }

    return result;  
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
    AST_Idx root = AST_MAKE(parser->tree, AST_SCOPE);

    AST_Idx current = AST_VALUE(parser->tree, root, AST_SCOPE).start = AST_MAKE(parser->tree, AST_CONS);
    AST_Idx prev = current;

    do {
        AST_VALUE(parser->tree, current, AST_CONS).data = Parser_Stmt(parser);

        AST_Idx next = AST_MAKE(parser->tree, AST_CONS);
        AST_VALUE(parser->tree, current, AST_CONS).next = next;
        prev = current;
        current = next;
    } while(!Parser_EOF(parser));

    AST_VALUE(parser->tree, prev, AST_CONS).next = AST_MAKE(parser->tree, AST_NULL);
    //GET_AST(parser->tree, current, AST_CONS).next = AST_MAKE(parser->tree, AST_NULL);
    
    return 0;
}

void Parser_run(Parser *parser) {
    parser->root = Parser_TopLevelScope(parser);
}
