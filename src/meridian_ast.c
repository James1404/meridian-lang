#include "meridian_ast.h"

#include <stdio.h>
#include <stdlib.h>

ASTList ASTList_make(void) {
    return (ASTList) {
        .data = NULL,
        .len = 0,
        .allocated = 8,
    };  
}

void ASTList_free(ASTList *list) {
    free(list->data);
}

AST_Idx ASTList_alloc(ASTList *list, AST node) {
    if(!list->data) {
        list->allocated = 8;
        list->data = malloc(sizeof(*list->data) * list->allocated);
    }

    AST_Idx idx = list->len++;

    if(list->len >= list->allocated) {
        list->allocated *= 2;
        list->data = realloc(list->data, sizeof(*list->data) * list->allocated);
    }

    list->data[idx] = node;

    return idx;
}

void ASTList_prettyPrint(ASTList* list, u64 indentation, AST_Idx node) {
    switch(AST_TY(list, node)) {
    case AST_NULL:
        printf("NULL");
        break;
    case AST_INTEGER:
        printf("%ld", AST_VALUE(list, node, AST_INTEGER));
        break;
    case AST_FLOAT:
        printf("%Lf", AST_VALUE(list, node, AST_FLOAT));
        break;
    case AST_BOOLEAN:
        printf("%s", AST_VALUE(list, node, AST_BOOLEAN) ? "true" : "false");
        break;

    case AST_STRING:
        printf("\"%.*s\"", AST_VALUE(list, node, AST_STRING).len, AST_VALUE(list, node, AST_STRING).raw);
        break;
        
    case AST_IDENT:
        printf("%.*s", AST_VALUE(list, node, AST_IDENT).len, AST_VALUE(list, node, AST_IDENT).raw);
        break;
        
    case AST_CONS:
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_CONS).data);
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_CONS).next);
        break;
        
    case AST_LIST:
        printf("(");
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_LIST).start);
        printf(")");
        break;
    case AST_SCOPE:
        printf("(");
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_SCOPE).start);
        printf(")");
        break;

    case AST_APPLICATION:
        printf("(");
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_APPLICATION).fn);

        printf(" ");
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_APPLICATION).arg);
        printf(")");
        break;
        
    case AST_ABSTRACTION:
        printf("(");
        printf("fn ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_ABSTRACTION).arg);

        printf(" -> ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_ABSTRACTION).body);
        printf(")");
        break;

        
    case AST_LET:
        printf("(");
        printf("let ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_LET).name);

        printf(" = ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_LET).value);
        
        printf(" in ");

        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_LET).in);

        printf(")");
        break;
    case AST_IF:
        printf("(");
        printf("if ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_IF).cond);

        printf(" then ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_IF).t);
        
        printf(" else ");

        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_IF).f);

        printf(")");
        break;
        
    case AST_DEFINE:
        printf("define ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_DEFINE).name);

        printf(" = ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_DEFINE).body);
        
        printf(";");
        break;
    case AST_TYPEDEF:
        printf("type ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_TYPEDEF).name);

        printf(" = ");
        
        ASTList_prettyPrint(list, indentation, AST_VALUE(list, node, AST_TYPEDEF).type);
        
        printf(";");
        break;
    }
}
