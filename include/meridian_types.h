#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_string.h"

typedef u64 TypeIndex;

typedef enum {
    TYPE_NOT_FOUND,
    TYPE_UNKNOWN,

    TYPE_UNIT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_STRING,

    TYPE_FN,

    TYPE_EXISTS,
    TYPE_FORALL,
} TypeTag;

typedef struct {
    TypeTag tag;
    union {
        struct {
            TypeIndex arg, body;
        } TYPE_FN;
    } payload;
} Type;

#define TYPE_MAKE(env, tag) (TypeAllocator_alloc((env), (Type) {tag}))
#define TYPE_MAKE_V(env, tag, value) (TypeAllocator_alloc((env), (Type){tag, .payload.tag = value}))
#define TYPE_MAKE_S(env, tag, ...) (TypeAllocator_alloc((env), (Type){tag, .payload.tag = {__VA_ARGS__}}))

#define TYPE_GET(env, idx) ((env)->data + idx)
#define TYPE_VALUE(env, idx, tag) (TYPE_GET(env, idx)->payload.tag)
#define TYPE_TAG(env, idx) (TYPE_GET(env, idx)->tag)

#define TYPE_SET(env, lhs, rhs) *(TYPE_GET(env, lhs)) = *(TYPE_GET(env, rhs))

typedef struct {
    Type* data;
    usize len, allocated;
} TypeAllocator;

TypeAllocator TypeAllocator_make(void);
void TypeAllocator_free(TypeAllocator *allocator);
TypeIndex TypeAllocator_alloc(TypeAllocator *allocator, Type type);

TypeIndex TypeAllocator_getTypeName(TypeAllocator *allocator, String name);

#endif//MERIDIAN_TYPES_H
