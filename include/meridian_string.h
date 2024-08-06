#ifndef MERIDIAN_STRING_H
#define MERIDIAN_STRING_H

#include "meridian_common.h"

#define STR(x) ((String) { .raw = x, .len = strlen(x) })

typedef struct {
    char* raw;
    u64 len;
} String;

#endif// MERIDIAN_STRING_H
