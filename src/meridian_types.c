#include "meridian_types.h"
#include "meridian_ast.h"

#include <stdio.h>
#include <stdlib.h>

TypeAllocator TypeAllocator_make(void) {
    return (TypeAllocator) {
        .data = NULL,
        .len = 0,
        .allocated = 8,
    };
}

void TypeAllocator_free(TypeAllocator *allocator) {
    free(allocator->data);
}

TypeIndex TypeAllocator_alloc(TypeAllocator* allocator, Type type) {
    if(!allocator->data) {
        allocator->allocated = 8;
        allocator->data = malloc(sizeof(*allocator->data) * allocator->allocated);
    }

    if(allocator->len >= allocator->allocated) {
        allocator->allocated *= 2;
        allocator->data = realloc(allocator->data, sizeof(*allocator->data) * allocator->allocated);
    }

    NodeIndex idx = allocator->len++;
    allocator->data[idx] = type;

    return idx;
}
