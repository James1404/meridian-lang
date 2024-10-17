#ifndef MERIDIAN_AST_H
#define MERIDIAN_AST_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_tokens.h"
#include "meridian_arena.h"

typedef u64 NodeIndex;

typedef struct {
  NodeIndex data, next;
} Cons;

typedef struct Node {
    enum {
        NODE_NULL,
        
        NODE_INTEGER,
        NODE_FLOAT,

        NODE_BOOLEAN,
        NODE_STRING,

        NODE_IDENT,

        NODE_CONS,
        
        NODE_LIST,

        NODE_SCOPE,

        NODE_APPLICATION,
        NODE_ABSTRACTION,

        NODE_LET,

        NODE_IF,
        
        NODE_DEFINE,
        NODE_TYPEDEF,

        NODE_ANNOTATE,

        NODE_APPLICATION_TYPE,
    } tag;
    union {
        i64 NODE_INTEGER;
        f64 NODE_FLOAT;
        bool NODE_BOOLEAN;
        String NODE_STRING;

        String NODE_IDENT;

        Cons NODE_CONS;
        
        NodeIndex NODE_LIST;
        NodeIndex NODE_SCOPE;
        
        struct {
            NodeIndex fn;
            NodeIndex arg;
        } NODE_APPLICATION;
        struct {
            NodeIndex arg;
            NodeIndex body;
        } NODE_ABSTRACTION;
        

        struct {
            NodeIndex name;
            NodeIndex value;

            NodeIndex in;
        } NODE_LET;

        struct {
            NodeIndex cond, t, f;
        } NODE_IF;

        struct {
            NodeIndex name, body;
        } NODE_DEFINE;
        struct {
            NodeIndex name, type;
        } NODE_TYPEDEF;

        struct {
            NodeIndex expression, type;
        } NODE_ANNOTATE;

        struct { NodeIndex fn, arg; } NODE_APPLICATION_TYPE;
    } payload;

    Token token;
} Node;

typedef struct {
    Node* data;
    NodeIndex len, allocated;
} NodeList;

NodeList NodeList_make(void);
void NodeList_free(NodeList *list);

NodeIndex NodeList_alloc(NodeList *list, Node node);

void NodeList_prettyPrint(NodeList* list, NodeIndex root);

#define NODE_MAKE(token, env, tag)                                             \
  (NodeList_alloc((env), (Node){tag, {0}, token}))
#define NODE_MAKE_V(token, env, tag, value)                                    \
  (NodeList_alloc((env), (Node){tag, {.tag = value}, token}))
#define NODE_MAKE_S(token, env, tag, ...)                                      \
  (NodeList_alloc((env), (Node){tag, {.tag = {__VA_ARGS__}}, token}))

#define NODE_GET(env, idx) ((env)->data + idx)

#define NODE_TY(env, idx) (NODE_GET((env), idx)->tag)
#define NODE_VALUE(env, idx, tag) (NODE_GET((env), idx)->payload.tag)

#endif//MERIDIAN_AST_H
