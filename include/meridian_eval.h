#ifndef MERIDIAN_EVAL_H
#define MERIDIAN_EVAL_H

#include "meridian_ast.h"
#include "meridian_string.h"

typedef struct {
    String id;
    AST_Idx node;
    u64 scope;
} Local;

typedef struct {
    Local* locals;
    u64 len, allocated, scope;
} Env;

Env Env_make(void);
void Env_free(Env* env);

void Env_inc(Env *env);
void Env_dec(Env *env);

void Env_set(Env *env, String id, AST_Idx node);
AST_Idx Env_get(Env* env, String id);

typedef struct {
    Env environment;
    ASTList* tree;
    AST_Idx root;
} Eval;

Eval Eval_make(ASTList *tree, AST_Idx root);
void Eval_free(Eval* eval);

#endif//MERIDIAN_EVAL_H
