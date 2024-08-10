#include "meridian_eval.h"
#include <stdlib.h>

Env Env_make(void) {
    return (Env) {
        .locals = NULL,
        .scope = 0,
        .len = 0,
        .allocated = 8,
    };
}

void Env_free(Env* env) {
    free(env->locals);
}

void Env_inc(Env *env) {
    env->scope++;  
}

void Env_dec(Env* env) {
    env->scope--;

    for(u64 i = env->len - 1; i > 0; i--) {
        if(env->locals[i].scope <= env->scope) {
            break;
        }
        
        env->len--;
    }
}
