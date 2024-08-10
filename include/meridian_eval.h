#ifndef MERIDIAN_EVAL_H
#define MERIDIAN_EVAL_H

#include "meridian_ast.h"

typedef struct {
    AST_Idx* node;
    u64 scope;
} Local;

typedef struct {
    Local* locals;
    u64 len, allocated, scope;
} Env;

Env Env_make(void);
void Env_free(Env* env);

void Env_inc(Env *env);
void Env_dec(Env* env);

#endif//MERIDIAN_EVAL_H
