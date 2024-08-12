#ifndef MERIDIAN_AST_H
#define MERIDIAN_AST_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_tokens.h"

typedef u64 AST_Idx;

typedef struct AST {
    enum {
        AST_NULL,
        
        AST_INTEGER,
        AST_FLOAT,

        AST_BOOLEAN,
        AST_STRING,

        AST_IDENT,
        
        AST_CONS,

        AST_LIST,

        AST_SCOPE,

        AST_APPLICATION,
        AST_ABSTRACTION,

        AST_LET,

        AST_IF,
        
        AST_DEFINE,
        AST_TYPEDEF,

        AST_ANNOTATE,
    } tag;
    union {
        i64 AST_INTEGER;
        f64 AST_FLOAT;
        bool AST_BOOLEAN;
        String AST_STRING;

        String AST_IDENT;

        struct {
            AST_Idx data, next;
        } AST_CONS;
        
        struct {
            AST_Idx start;
        } AST_LIST;
        
        struct {
            AST_Idx start;
        } AST_SCOPE;
        
        struct {
            AST_Idx fn;
            AST_Idx arg;
        } AST_APPLICATION;
        struct {
            AST_Idx arg;
            AST_Idx body;
        } AST_ABSTRACTION;
        

        struct {
            AST_Idx name;
            AST_Idx value;

            AST_Idx in;
        } AST_LET;

        struct {
            AST_Idx cond, t, f;
        } AST_IF;

        struct {
            AST_Idx name;
            AST_Idx body;
        } AST_DEFINE;
        struct {
            AST_Idx name;
            AST_Idx type;
        } AST_TYPEDEF;

        struct {
            AST_Idx expression, type;
        } AST_ANNOTATE;
    } payload;
} AST;

typedef struct {
    AST* data;
    AST_Idx len, allocated;
} ASTList;

ASTList ASTList_make(void);
void ASTList_free(ASTList *list);

AST_Idx ASTList_alloc(ASTList *list, AST node);

void ASTList_prettyPrint(ASTList* list, u64 indentation, AST_Idx node);

#define AST_MAKE(list, tag) (ASTList_alloc(list, (AST){tag, {0}}))
#define AST_MAKE_V(list, tag, value)                                           \
  (ASTList_alloc(list, (AST){tag, {.tag = value}}))
#define AST_MAKE_S(list, tag, ...)                                             \
  (ASTList_alloc(list, (AST){tag, {.tag = {__VA_ARGS__}}}))

#define AST_GET(list, idx) ((list)->data + idx)

#define AST_TY(list, idx) (AST_GET(list, idx)->tag)
#define AST_VALUE(list, idx, tag) (AST_GET(list, idx)->payload.tag)

#endif//MERIDIAN_AST_H
