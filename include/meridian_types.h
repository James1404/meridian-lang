#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_ast.h"

typedef u64 TypeIdx;

typedef enum {
    TYPE_UNKNOWN,
    
    TYPE_ID,

    TYPE_UNIT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOLEAN,
    TYPE_STRING,
} TypeTag;

typedef struct {
    TypeTag tag;
    union {
        u64 TYPE_ID;
    } payload;
} Type;

#define TYPE_MAKE(tag) ((Type){tag, {0}})
#define TYPE_MAKE_V(tag, value) ((Type){tag, {.tag = value}})
#define TYPE_MAKE_S(tag, ...) ((Type){tag, {.tag = {__VA_ARGS__}}})

String Type_toString(Type ty);

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

Type TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, Type expected);
Type TypeInfer(TypeEnv *env, ASTList *tree, AST_Idx node);

Type GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node);

void TypeCheckScope(ASTList* tree, AST_Idx node);

Type GetTypeName(String id);

void RunTypeChecker(ASTList* tree, AST_Idx root);

#endif//MERIDIAN_TYPES_H
