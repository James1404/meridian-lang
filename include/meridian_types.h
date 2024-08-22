#ifndef MERIDIAN_TYPES_H
#define MERIDIAN_TYPES_H

#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_ast.h"
#include "meridian_arena.h"

typedef u64 TypeIdx;

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
            TypeIdx arg, body;
        } TYPE_FN;

        struct {
            bool solved;
            TypeIdx monotype;
        } TYPE_EXISTS;
        struct {} TYPE_FORALL;
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

    u64 universalQuantifiers;

    bool complete;

    Arena arena;
} TypeEnv;

TypeEnv TypeEnv_make(void);
void TypeEnv_free(TypeEnv *env);

TypeIdx TypeEnv_alloc(TypeEnv* env, Type ty);

bool TypeEnv_isMonoType(TypeEnv *env, TypeIdx i);
bool TypeEnv_isPolyType(TypeEnv *env, TypeIdx i);

bool TypeEnv_eq(TypeEnv* env, TypeIdx l, TypeIdx r);
String TypeEnv_toString(TypeEnv* env, TypeIdx ty);

void TypeEnv_inc(TypeEnv *env);
void TypeEnv_dec(TypeEnv* env);

void TypeEnv_set(TypeEnv *env, String name, TypeIdx ty);
TypeIdx TypeEnv_get(TypeEnv *env, String name);
bool TypeEnv_has(TypeEnv* env, String name);

bool TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, TypeIdx expected);
TypeIdx TypeInfer(TypeEnv *env, ASTList *tree, AST_Idx node);

void TypeEnv_insertAST(TypeEnv* env, ASTList* tree, AST_Idx node, TypeIdx type);
TypeIdx GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node);

void TypeCheckScope(ASTList* tree, AST_Idx node);

bool TypeEnv_occursCheck(TypeEnv* env, TypeIdx a, TypeIdx against);

void TypeEnv_instantiateL(TypeEnv *env0, TypeIdx alpha, TypeIdx mono0);
void TypeEnv_instantiateR(TypeEnv* env, TypeIdx mono, TypeIdx alpha);

TypeIdx TypeEnv_GetTypeName(TypeEnv* env, String id);

void RunTypeChecker(ASTList* tree, AST_Idx root);

#endif//MERIDIAN_TYPES_H
