#ifndef MERIDIAN_AST_H
#define MERIDIAN_AST_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_tokens.h"

typedef struct Ast {
    enum {
        AST_INTEGER,
        AST_FLOAT,

        AST_BOOLEAN,
        AST_STRING,

        AST_IDENT,
        
        AST_BINARY,
        AST_UNARY,

        AST_LIST,

        AST_APPLICATION,
        AST_ABSTRACTION,

        AST_LET,
        
        AST_DEFINE,
        AST_TYPEDEF,
    } tag;
    union {
        i64 AST_INTEGER;
        f64 AST_FLOAT;
        bool AST_BOOLEAN;
        String AST_STRING;

        String AST_IDENT;

        struct {
            struct Ast* lhs;
            struct Ast* rhs;
            Token op;
        } AST_BINARY;
        struct {
            Token op;
            struct Ast* node;
        } AST_UNARY;
        
        struct {
            struct Ast* data;
            u64 len, allocated;
        } AST_LIST;
        
        struct {
            struct Ast* fn;
            struct Ast* args;
            u64 argc;
        } AST_APPLICATION;
        struct {
            struct Ast* args;
            u64 argc;

            struct Ast* body;
        } AST_ABSTRACTION;

        struct {
            struct Ast* name;
            struct Ast* value;

            struct Ast* in;
        } AST_LET;

        struct {
            struct Ast* name;
            struct Ast* body;
        } AST_DEFINE;
        struct {
            struct Ast* name;
            struct Ast* type;
        } AST_TYPEDEF;
    } payload;
} Ast;

void Ast_prettyPrint(u64 indentation, Ast* node);

#endif//MERIDIAN_AST_H
