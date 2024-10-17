#include "meridian_ast.h"
#include "meridian_arena.h"

#include <stdio.h>
#include <stdlib.h>

NodeList NodeList_make(void) {
    return (NodeList) {
        .data = NULL,
        .len = 0,
        .allocated = 8,
    };  
}

void NodeList_free(NodeList *list) {
    free(list->data);
}

NodeIndex NodeList_alloc(NodeList *list, Node node) {
    if(!list->data) {
        list->allocated = 8;
        list->data = malloc(sizeof(*list->data) * list->allocated);
    }

    NodeIndex idx = list->len++;

    if(list->len >= list->allocated) {
        list->allocated *= 2;
        list->data = realloc(list->data, sizeof(*list->data) * list->allocated);
    }

    list->data[idx] = node;

    return idx;
}

static void NodeList_prettyPrintNode(NodeList* list, NodeIndex node) {
    switch(NODE_TY(list, node)) {
    case NODE_NULL:
        printf("NULL");
        break;
    case NODE_INTEGER:
        printf("%ld", NODE_VALUE(list, node, NODE_INTEGER));
        break;
    case NODE_FLOAT:
        printf("%Lf", NODE_VALUE(list, node, NODE_FLOAT));
        break;
    case NODE_BOOLEAN:
        printf("%s", NODE_VALUE(list, node, NODE_BOOLEAN) ? "true" : "false");
        break;

    case NODE_STRING:
        printf("\"%.*s\"", NODE_VALUE(list, node, NODE_STRING).len, NODE_VALUE(list, node, NODE_STRING).raw);
        break;
        
    case NODE_IDENT:
        printf("%.*s", NODE_VALUE(list, node, NODE_IDENT).len, NODE_VALUE(list, node, NODE_IDENT).raw);
        break;
        
    case NODE_CONS:
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_CONS).data);
        NodeIndex next = NODE_VALUE(list, node, NODE_CONS).next;
        if(NODE_TY(list, next) != NODE_NULL) {
            NodeList_prettyPrintNode(list, next);
        }
        break;
        
    case NODE_LIST:
        printf("(");
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_LIST));
        printf(")");
        break;
    case NODE_SCOPE:
        printf("(");
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_SCOPE));
        printf(")");
        break;

    case NODE_APPLICATION:
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_APPLICATION).fn);

        printf("(");
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_APPLICATION).arg);
        printf(")");
        break;
        
    case NODE_ABSTRACTION:
        printf("(");
        printf("fn ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_ABSTRACTION).arg);

        printf(" -> ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_ABSTRACTION).body);
        printf(")");
        break;

        
    case NODE_LET:
        printf("(");
        printf("let ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_LET).name);

        printf(" = ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_LET).value);
        
        printf(" in ");

        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_LET).in);

        printf(")");
        break;
    case NODE_IF:
        printf("(");
        printf("if ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_IF).cond);

        printf(" then ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_IF).t);
        
        printf(" else ");

        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_IF).f);

        printf(")");
        break;
        
    case NODE_DEFINE:
        printf("define ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_DEFINE).name);

        printf(" = ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_DEFINE).body);
        
        printf(";");
        break;
    case NODE_TYPEDEF:
        printf("type ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_TYPEDEF).name);

        printf(" = ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_TYPEDEF).type);
        
        printf(";");
        break;
    case NODE_ANNOTATE:
        printf("(");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_ANNOTATE).expression);
        
        printf(" :: ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_ANNOTATE).type);
       
        printf(")");
        break;
    case NODE_APPLICATION_TYPE:
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_APPLICATION_TYPE).fn);
        
        printf(" -> ");
        
        NodeList_prettyPrintNode(list, NODE_VALUE(list, node, NODE_APPLICATION_TYPE).arg);
        break;
    }
}

void NodeList_prettyPrint(NodeList* list,  NodeIndex root) {
    NodeList_prettyPrintNode(list, root);
    printf("\n");
}
