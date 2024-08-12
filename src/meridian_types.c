#include "meridian_types.h"
#include "meridian_error.h"
#include <stdlib.h>


TypeList TypeList_fromAST(ASTList *tree, AST_Idx root) {
    return (TypeList) {
        .data = NULL,
        .len = 0,
        .allocated = 0,
        .currentTypeId = 0,
    };
}
void TypeList_free(TypeList *list) {
    free(list->data);
}

TypeIdx TypeList_alloc(TypeList *list, Type t) {
    if(!list->data) {
        list->allocated = 8;
        list->data = malloc(sizeof(*list->data) * list->allocated);
    }

    AST_Idx idx = list->len++;

    if(list->len >= list->allocated) {
        list->allocated *= 2;
        list->data = realloc(list->data, sizeof(*list->data) * list->allocated);
    }

    list->data[idx] = t;

    return idx;
}

TypeEnv TypeEnv_make(void) {
    return (TypeEnv) {
        .locals = NULL,
        .len = 0,
        .allocated = 0,
        .scope = 0,
    };
}

void TypeEnv_free(TypeEnv *env) { free(env->locals); }

void TypeEnv_inc(TypeEnv *env) { env->scope++; }
void TypeEnv_dec(TypeEnv* env) {
    env->scope--;
    
    while(env->locals[env->len - 1].scope > env->scope) {
        env->len--;
    }
}

void TypeEnv_set(TypeEnv *env, String name, Type ty) {
    if(!env->locals) {
        env->len = 0;
        env->allocated = 8;
        env->locals = malloc(sizeof(*env->locals) * env->allocated);
    }
    
    if(env->len >= env->allocated) {
        env->allocated *= 2;

        LocalType* temp = realloc(env->locals, sizeof(LocalType) * env->allocated);
        if(temp) {
            env->locals = temp;
        }
        else {
            Meridian_error("%s :: realloc error", "TypeEnv_set");
            return;
        }
    }

    LocalType* local = env->locals + (env->len++);
    *local = (LocalType) {
        .scope = env->scope,
        .ty = ty,
    };
    
    STR_CPY_ALLOC(local->name, name);
}
Type TypeEnv_get(TypeEnv* env, String name) {
    for(i64 i = env->len - 1; i >= 0; i--) {
        LocalType entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.ty;
        }
    }

    Meridian_error("Could not find symbol in type environment");
    return (Type) { 0 };
}
    

TypeChecker TypeChecker_make(void) {
    return (TypeChecker) {
        .todo = 0
    };
}
void TypeChecker_free(TypeChecker *checker);

void TypeChecker_run(TypeChecker* checker);
