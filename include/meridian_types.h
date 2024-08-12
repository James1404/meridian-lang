#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_ast.h"

typedef u64 TypeIdx;

typedef struct {
    TypeIdx parent;
    
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
    Type* data;
    u64 len, allocated;

    u64 currentTypeId;
} TypeList;

TypeList TypeList_fromAST(ASTList *tree, AST_Idx root);
void TypeList_free(TypeList *list);

TypeIdx TypeList_alloc(TypeList *list, Type t);

#define TYPE_MAKE(list, parent, tag) (TypeLst_alloc(list, (Type){parent,tag, {0}}))
#define TYPE_MAKE_V(list, parent, tag, value)                                  \
  (TypeLst_alloc(list, (Type){parent, tag, {.tag = value}}))
#define TYPE_MAKE_S(list, parent, tag, ...)                                    \
  (TypeLst_alloc(list, (Type){parent, tag, {.tag = {__VA_ARGS__}}}))

#define TYPE_GET(list, idx) ((list)->data + idx)

#define TYPE_TAG(list, idx) (TYPE_GET(list, idx)->tag)
#define TYPE_VALUE(list, idx, tag) (TYPE_GET(list, idx)->payload.tag)

typedef struct {
    String name;
    
    Type ty;
    u64 scope;
} LocalType;

typedef struct {
    LocalType* locals;
    u64 len, allocated, scope;
} TypeEnv;

TypeEnv TypeEnv_make(void);
void TypeEnv_free(TypeEnv *env);

void TypeEnv_inc(TypeEnv *env);
void TypeEnv_dec(TypeEnv* env);

void TypeEnv_set(TypeEnv *env, String name, Type ty);
Type TypeEnv_get(TypeEnv* env, String name);

typedef struct {
    u64 todo;
} TypeChecker;

TypeChecker TypeChecker_make(void);
void TypeChecker_free(TypeChecker *checker);

void TypeChecker_run(TypeChecker* checker);

#endif//MERIDIAN_TYPES_H
