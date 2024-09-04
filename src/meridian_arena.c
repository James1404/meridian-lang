#include "meridian_arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static ArenaPage* ArenaPage_make(void) {
    ArenaPage* page = malloc(sizeof(*page));
    *page = (ArenaPage) {
        .mem = malloc(PAGE_SIZE),
        .used = 0,
        .next = NULL
    };

    return page;
}

static void ArenaPage_free(ArenaPage* page) {
    if(page->next) {
        ArenaPage_free(page->next);
    }

    free(page->mem);
    
    free(page);
}

Arena Arena_make(void) {
    ArenaPage* start = ArenaPage_make();
    return (Arena) {
        .start = start,
        .end = start,
    };
}
void Arena_free(Arena *arena) {
    ArenaPage_free(arena->start);
    arena->start = arena->end = NULL;
}

void* Arena_alloc(Arena* arena, usize size) {
    ArenaPage* page = arena->end;
    
    if(size <= (PAGE_SIZE - page->used)) {
        arena->end = page->next = ArenaPage_make();
        page = page->next;
    }

    u8* ptr = page->mem + page->used;

    page->used += size;
    
    return ptr;
}

String Arena_fmt(Arena* arena, const char* fmt, ...) {
    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);

    u64 len = vsnprintf(NULL, 0, fmt, args);

    char* buffer = Arena_alloc(arena, len + 1);

    vsnprintf(buffer, len + 1, fmt, args2);

    va_end(args);
    va_end(args2);

    return (String) { buffer, len };
}
