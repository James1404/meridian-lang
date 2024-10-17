#include "meridian_parser.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include "meridian_tokens.h"

Parser Parser_make(TokenList *in, NodeList* out) {
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

NodeIndex Parser_Ident(Parser *parser) {
    Token t = Parser_current(parser);

    if(!Parser_match(parser, TOKEN_IDENT)) {
        return NODE_MAKE(t, parser->tree, NODE_NULL);
    }
    
    NodeIndex id = NODE_MAKE(t, parser->tree, NODE_IDENT);
    String text = { parser->tokens->src.raw + t.start, t.len };
    NODE_VALUE(parser->tree, id, NODE_IDENT) = text;

    return id;
}

NodeIndex Parser_Value(Parser *parser) {
    Token start = Parser_current(parser);
    
    switch(start.tt) {
    case TOKEN_IDENT: {
        return Parser_Ident(parser);
    }
        
    case TOKEN_INT: {
        String text = TOKENLIST_text(parser->tokens, Parser_current(parser));

        String cpy;
        STR_CPY_ALLOC_NULL(cpy, text);

        NodeIndex result = NODE_MAKE_V(start, parser->tree, NODE_INTEGER, strtoll(cpy.raw, NULL, 10));

        STR_FREE(cpy);

        Parser_adv(parser);
        return result;
    }
    case TOKEN_FLOAT: {
        String text = TOKENLIST_text(parser->tokens, Parser_current(parser));

        String cpy;
        STR_CPY_ALLOC_NULL(cpy, text);

        NodeIndex result = NODE_MAKE_V(start, parser->tree, NODE_FLOAT, strtof(cpy.raw, NULL));

        STR_FREE(cpy);

        Parser_adv(parser);
        return result;
    }

    case TOKEN_TRUE: {
        Parser_adv(parser);
        return NODE_MAKE_V(start, parser->tree, NODE_BOOLEAN, true);
    }
    case TOKEN_FALSE: {
        Parser_adv(parser);
        return NODE_MAKE_V(start, parser->tree, NODE_BOOLEAN, false);
    }

    case TOKEN_STRING: {
        NodeIndex result = NODE_MAKE_V(start, parser->tree, NODE_STRING, TOKENLIST_text(parser->tokens, Parser_current(parser)));
        Parser_adv(parser);
        return result;
    }

    case TOKEN_LET: {
        return Parser_Let(parser);
    }
    case TOKEN_IN: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'in'");
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }

    case TOKEN_IF: {
        return Parser_If(parser);
    }
    case TOKEN_THEN: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'then'");
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }
    case TOKEN_ELSE: {
        Parser_adv(parser);
        Meridian_error("Unexpected 'else'");
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }

    case TOKEN_LPAREN: {
        Parser_adv(parser);
        NodeIndex result = Parser_Expression(parser);
        if(!Parser_match(parser, TOKEN_RPAREN)) {
            Meridian_error("Expected closing ')'");
            return NODE_MAKE(start, parser->tree, NODE_NULL);
        }
        
        return result;
    }
    case TOKEN_RPAREN: {
        Parser_adv(parser);
        Meridian_error("Unexpected ')'");
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }

    case TOKEN_FN: {
        return Parser_Abstraction(parser);
    }
        
    default: {
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }
    }
}

NodeIndex Parser_Let(Parser *parser) {
    Token start = Parser_current(parser);
    
    if(!Parser_match(parser, TOKEN_LET)) {
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex name = Parser_Ident(parser);

    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in let");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex value = Parser_Expression(parser);

    if(!Parser_match(parser, TOKEN_IN)) {
        Meridian_error("Expected 'in' in let");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex in = Parser_Expression(parser);

    return NODE_MAKE_S(start, parser->tree, NODE_LET, name, value, in);
}

NodeIndex Parser_If(Parser *parser) {
    Token start = Parser_current(parser);
    
    if(!Parser_match(parser, TOKEN_IF)) {
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex cond = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_THEN)) {
        Meridian_error("Expected 'then' in if expression");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex t = Parser_Expression(parser);
    
    if(!Parser_match(parser, TOKEN_ELSE)) {
        Meridian_error("Expected 'else' in if expression");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex f = Parser_Expression(parser);

    return NODE_MAKE_S(start, parser->tree, NODE_IF, cond, t, f);
}

NodeIndex Parser_Type(Parser *parser) {
    Token start = Parser_current(parser);

    NodeIndex ty = 0;
    switch(start.tt) {
    case TOKEN_IDENT:
        ty = Parser_Ident(parser);
        break;
    default:
        Parser_adv(parser);
        return NODE_MAKE(start, parser->tree, NODE_NULL);
    }

    if(Parser_match(parser, TOKEN_ARROW)) {
        return NODE_MAKE_S(start, parser->tree, NODE_APPLICATION_TYPE, ty, Parser_Type(parser));
    }

    return ty;
}

NodeIndex Parser_Expression(Parser* parser) {
    Token start = Parser_current(parser);
    NodeIndex first = Parser_Value(parser);

    if(Parser_match(parser, TOKEN_ANNOTATE)) {
        NodeIndex type = Parser_Type(parser);
        return NODE_MAKE_S(start, parser->tree, NODE_ANNOTATE, first, type);
    }

    NodeIndex arg = Parser_Value(parser);

    if(NODE_TY(parser->tree, arg) != NODE_NULL) {
        return NODE_MAKE_S(start, parser->tree, NODE_APPLICATION, first, arg);
    }

    return first;
}

NodeIndex Parser_Abstraction(Parser* parser) {
    Token start = Parser_current(parser);
    if(!Parser_match(parser, TOKEN_FN)) {
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex arg = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ARROW)) {
        Meridian_error("Expected '->' in fn abstraction");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex body = Parser_Expression(parser);

    return NODE_MAKE_S(start, parser->tree, NODE_ABSTRACTION, arg, body);
}

NodeIndex Parser_Define(Parser *parser) {
    Token start = Parser_current(parser);
    if(!Parser_match(parser, TOKEN_DEF)) {
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in definition");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex body = Parser_Expression(parser);

    return NODE_MAKE_S(start, parser->tree, NODE_DEFINE, name, body);
}

NodeIndex Parser_TypeDef(Parser *parser) {
    Token start = Parser_current(parser);
    if(!Parser_match(parser, TOKEN_TYPE)) {
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex name = Parser_Ident(parser);
    
    if(!Parser_match(parser, TOKEN_ASSIGN)) {
        Meridian_error("Expected '=' in type definition");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    NodeIndex type = Parser_Ident(parser);
    
    return NODE_MAKE_S(start, parser->tree, NODE_TYPEDEF, name, type);  
}

NodeIndex Parser_Stmt(Parser *parser) {
    NodeIndex stmt = NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    
    switch(Parser_current(parser).tt) {
    case TOKEN_DEF:
        stmt = Parser_Define(parser);
        break;
    case TOKEN_TYPE:
        stmt = Parser_TypeDef(parser);
        break;
    default:
        stmt = Parser_Expression(parser);
        break;
    }
    
    if(!Parser_match(parser, TOKEN_SEMICOLON)) {
        Meridian_error("Expected ';' at the end of a statement");
        return NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    }

    return stmt;
}

NodeIndex Parser_TopLevelScope(Parser* parser) {
    NodeIndex root = NODE_MAKE_V(Parser_current(parser), parser->tree, NODE_SCOPE, 0);

    NodeIndex current = NODE_VALUE(parser->tree, root, NODE_SCOPE) = NODE_MAKE_S(Parser_current(parser), parser->tree, NODE_CONS, 0, 0);
    NodeIndex prev = current;

    do {
        NodeIndex data = Parser_Stmt(parser);
        NODE_VALUE(parser->tree, current, NODE_CONS).data = data;

        NodeIndex next = NODE_MAKE_S(Parser_current(parser), parser->tree, NODE_CONS, 0, 0);
        NODE_VALUE(parser->tree, current, NODE_CONS).next = next;
        prev = current;
        current = next;
    } while(!Parser_EOF(parser));

    NODE_VALUE(parser->tree, prev, NODE_CONS).next = NODE_MAKE(Parser_current(parser), parser->tree, NODE_NULL);
    
    return root;
}

void Parser_run(Parser *parser) {
    parser->root = Parser_TopLevelScope(parser);
}
