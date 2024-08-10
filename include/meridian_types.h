#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
typedef u64 TypeIdx;

typedef struct {
    enum {
        TYPE_ID,

        TYPE_UNIT,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_BOOLEAN,
        TYPE_STRING,
    } tag;
    union {
        u64 TYPE_ID;
    } payload;
} Type;

typedef struct {
  
} TypeList;

#endif//MERIDIAN_TYPES_H
