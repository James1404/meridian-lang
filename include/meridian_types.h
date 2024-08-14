#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_ast.h"
#include "meridian_arena.h"

typedef u64 TypeIdx;

typedef enum {
    TYPE_UNKNOWN,
    
    TYPE_ID,

    TYPE_UNIT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_STRING,

    TYPE_FN
} TypeTag;

typedef struct {
    TypeTag tag;
    union {
        u64 TYPE_ID;
        struct {
            TypeIdx arg, body;
        } TYPE_FN;
    } payload;
} Type;

#define TYPE_MAKE(env, tag) (TypeEnv_alloc((env), (Type){tag, {0}}))
#define TYPE_MAKE_V(env, tag, value) (TypeEnv_alloc((env), (Type){tag, {.tag = value}}))
#define TYPE_MAKE_S(env, tag, ...) (TypeEnv_alloc((env), (Type){tag, {.tag = {__VA_ARGS__}}}))

#define TYPE_GET(env, idx) ((env)->typelist + idx)
#define TYPE_VALUE(env, idx, tag) (TYPE_GET(env, idx)->payload.tag)
#define TYPE_TAG(env, idx) (TYPE_GET(env, idx)->tag)

typedef struct {
    String name;
    
    TypeIdx ty;
    u64 scope;
} LocalType;

typedef struct {
    LocalType* locals;
    u64 len, allocated, scope;

    Type* typelist;
    u64 typelistLen, typelistAllocated;

    Arena allocator;
} TypeEnv;

TypeEnv TypeEnv_make(void);
void TypeEnv_free(TypeEnv *env);

TypeIdx TypeEnv_alloc(TypeEnv* env, Type ty);

bool TypeEnv_eq(TypeEnv* env, TypeIdx l, TypeIdx r);
String TypeEnv_toString(TypeEnv* env, TypeIdx ty);

void TypeEnv_inc(TypeEnv *env);
void TypeEnv_dec(TypeEnv* env);

void TypeEnv_set(TypeEnv *env, String name, TypeIdx ty);
TypeIdx TypeEnv_get(TypeEnv* env, String name);

TypeIdx TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, TypeIdx expected);
TypeIdx TypeInfer(TypeEnv *env, ASTList *tree, AST_Idx node);

TypeIdx GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node);

void TypeCheckScope(ASTList* tree, AST_Idx node);

TypeIdx TypeEnv_GetTypeName(TypeEnv* env, String id);

void RunTypeChecker(ASTList* tree, AST_Idx root);

#endif//MERIDIAN_TYPES_H
