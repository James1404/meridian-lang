#include "meridian_env.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include "meridian_types.h"

#include <stdlib.h>

Env Env_make(void) {
    return (Env) {
        .locals = NULL,
        .len = 0,
        .allocated = 0,
        .scope = 0,

        .nodes = NodeList_make(),
    };
}

void Env_free(Env *env) {
    free(env->locals);
    
    NodeList_free(&env->nodes);
}

void Env_set(Env *env, String name, NodeIndex value, TypeIndex type) {
    if(!env->locals) {
        env->len = 0;
        env->allocated = 8;
        env->locals = malloc(sizeof(*env->locals) * env->allocated);
    }
    
    if(env->len >= env->allocated) {
        env->allocated *= 2;

        Local* temp = realloc(env->locals, sizeof(Local) * env->allocated);
        if(temp) {
            env->locals = temp;
        }
        else {
            Meridian_error("%s :: realloc error", "TypeEnv_set");
            return;
        }
    }

    Local* local = env->locals + (env->len++);
    *local = (Local) {
        .scope = env->scope,
        .value = value,
        .name = name,
    };
}

Local* Env_get(Env *env, String name) {    
    for(i64 i = env->len - 1; i >= 0; i--) {
        Local* entry = env->locals + i;

        if(STR_CMP(entry->name, name)) {
            return entry;
        }
    }

    Meridian_error("The symbol '%.*s' could not find within the current scope", name.len, name.raw);
    return NULL;
}

bool Env_has(Env *env, String name) {
    for(i64 i = env->len - 1; i >= 0; i--) {
        Local entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return true;
        }
    }

    return false;
}

void Env_increment(Env *env) { env->scope++; }
void Env_decrement(Env* env) {
    env->scope--;
    
    while(env->locals[env->len - 1].scope > env->scope) {
        env->len--;
    }
}
