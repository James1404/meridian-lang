#ifndef MERIDIAN_ARENA_H
#define MERIDIAN_ARENA_H

#include "meridian_common.h"
#include "meridian_string.h"

typedef struct ArenaPage {
    void* mem;
    usize used;

    struct ArenaPage* next;
} ArenaPage;

#define PAGE_SIZE (sizeof(u64) * 100)

typedef struct {
    ArenaPage* start, *end;
} Arena;

Arena Arena_make(void);
void Arena_free(Arena *arena);

void *Arena_alloc(Arena *arena, usize size);

String Arena_fmt(Arena* arena, const char* fmt, ...);

#endif//MERIDIAN_ARENA_H
