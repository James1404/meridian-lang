#include "meridian_tokens.h"

#include <stdlib.h>

TokenList TokenList_make(String src) {
    return (TokenList) {
        .data = NULL,
        .len = 0,
        .allocated = 0,
        .src = src,
    };
}

void TokenList_free(TokenList *list) {
    free(list->data);
}

void TokenList_push(TokenList* list, Token elem) {
    if(!list->data) {
        list->allocated = 8;
        list->data = malloc(sizeof(*list->data) * list->allocated);
    }

    u32 idx = list->len++;

    if(list->len >= list->allocated) {
        list->allocated *= 2;
        list->data = realloc(list->data, sizeof(*list->data) * list->allocated);
    }

    list->data[idx] = elem;
}
