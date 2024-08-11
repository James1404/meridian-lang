#ifndef MERIDIAN_STRING_H
#define MERIDIAN_STRING_H

#include "meridian_common.h"

#include <stdlib.h>
#include <string.h>

#define STR(x) ((String){.raw = x, .len = strlen(x)})
#define STR_ALLOC(l) ((String){.raw = malloc(l), .len = l});
#define STR_CPY_ALLOC(dst, src)                                                \
  do {                                                                         \
    dst = STR_ALLOC((src).len);                                                \
    strncpy((dst).raw, (src).raw, (src).len);                                  \
  } while (0)
#define STR_CPY_ALLOC_NULL(dst, src)                                                \
  do {                                                                         \
    dst = STR_ALLOC((src).len + 1);                                                \
    strncpy((dst).raw, (src).raw, (src).len);                                  \
    dst.raw[(src).len] = '\0';\
  } while (0)

#define STR_FREE(str)                                                          \
  do {                                                                         \
    free(str.raw);                                                             \
  } while (0)


typedef struct {
    char* raw;
    i32 len;
} String;

#define STR_CMP(lhs, rhs) (strncmp((lhs).raw, (rhs).raw, (lhs).len) == 0)

#endif// MERIDIAN_STRING_H
