#ifndef MERIDIAN_ENV_H
#define MERIDIAN_ENV_H

#include "meridian_ast.h"
#include "meridian_string.h"
#include "meridian_types.h"

typedef struct {
    String name;
    u64 scope;
    
    NodeIndex value;
} Local;

typedef struct {
    Local* locals;
    usize len, allocated, scope;

    NodeList nodes;

    Arena arena;
} Env;

Env Env_make(void);
void Env_free(Env *env);

void Env_set(Env *env, String name, NodeIndex value, TypeIndex type);
Local* Env_get(Env *env, String name);
bool Env_has(Env *env, String name);

void Env_increment(Env *env);
void Env_decrement(Env *env);

#endif//MERIDIAN_ENV_H
