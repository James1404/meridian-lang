#include "meridian_types.h"
#include "meridian_error.h"
#include "meridian_string.h"

#include <stdlib.h>

TypeEnv TypeEnv_make(void) {
    return (TypeEnv) {
        .locals = NULL,
        .len = 0,
        .allocated = 0,
        .scope = 0,

        .typelist = NULL,
        .typelistLen = 0,
        .typelistAllocated = 8,

        .allocator = Arena_make(),
    };
}

void TypeEnv_free(TypeEnv *env) {
    free(env->locals);
    free(env->typelist);

    Arena_free(&env->allocator);
}

TypeIdx TypeEnv_alloc(TypeEnv* env, Type ty) {
    if(!env->typelist) {
        env->typelistAllocated = 8;
        env->typelist = malloc(sizeof(*env->typelist) * env->typelistAllocated);
    }

    AST_Idx idx = env->typelistLen++;

    if(env->typelistLen >= env->typelistAllocated) {
        env->typelistAllocated *= 2;
        env->typelist = realloc(env->typelist, sizeof(*env->typelist) * env->typelistAllocated);
    }

    env->typelist[idx] = ty;

    return idx;
}

bool TypeEnv_eq(TypeEnv* env, TypeIdx l, TypeIdx r) {
    return TYPE_TAG(env, l) == TYPE_TAG(env, r);
}

String TypeEnv_toString(TypeEnv* env, TypeIdx ty) {
    switch(TYPE_TAG(env, ty)) {
    case TYPE_UNKNOWN: return STR("Unknown");

    case TYPE_ID: return STR("Id");
        
    case TYPE_UNIT: return STR("Unit");
    case TYPE_INT: return STR("Int");
    case TYPE_FLOAT: return STR("Float");
    case TYPE_BOOLEAN: return STR("Bool");
    case TYPE_STRING: return STR("String");

    case TYPE_FN: {
        String arg = TypeEnv_toString(env, TYPE_VALUE(env, ty, TYPE_FN).arg);
        String body = TypeEnv_toString(env, TYPE_VALUE(env, ty, TYPE_FN).body);
        return Arena_fmt(&env->allocator, "%.*s -> %.*s", arg, body);
    }
    }

    return STR("TypeError");
}

void TypeEnv_inc(TypeEnv *env) { env->scope++; }
void TypeEnv_dec(TypeEnv* env) {
    env->scope--;
    
    while(env->locals[env->len - 1].scope > env->scope) {
        env->len--;
    }
}

void TypeEnv_set(TypeEnv *env, String name, TypeIdx ty) {
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

TypeIdx TypeEnv_get(TypeEnv* env, String name) {
    TypeIdx ty = TypeEnv_GetTypeName(env, name);
    if(TYPE_TAG(env, ty) != TYPE_UNKNOWN) return ty;
    
    for(i64 i = env->len - 1; i >= 0; i--) {
        LocalType entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.ty;
        }
    }

    Meridian_error("The symbol '%.*s' could not be found in the current scope", name.len, name.raw);
    return TYPE_MAKE(env, TYPE_UNKNOWN);
}

TypeIdx TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, TypeIdx expected) {
    TypeIdx inferred = TypeInfer(env, tree, node);

    if(!TypeEnv_eq(env, inferred, expected)) {
        String inferredstr = TypeEnv_toString(env, inferred);
        String expectedstr = TypeEnv_toString(env, expected);
        Meridian_error("Expression of type '%.*s' does not match the annotation '%.*s'", inferredstr.len, inferredstr.raw, expectedstr.len, expectedstr.raw);
    }

    return inferred;
}

TypeIdx TypeInfer(TypeEnv* env, ASTList* tree, AST_Idx node) {
    switch(AST_TY(tree, node)) {
    case AST_INTEGER: return TYPE_MAKE(env, TYPE_INT);
    case AST_FLOAT: return TYPE_MAKE(env, TYPE_FLOAT);
    case AST_BOOLEAN: return TYPE_MAKE(env, TYPE_BOOLEAN);
    case AST_STRING: return TYPE_MAKE(env, TYPE_STRING);
    case AST_IDENT: {
        String id = AST_VALUE(tree, node, AST_IDENT);
        TypeIdx t = TypeEnv_get(env, id);

        if(TYPE_TAG(env, t) == TYPE_UNKNOWN) {
            Meridian_error("Type identifier does not exist '%.*s'", id.len, id.raw);
        }

        return t;
    }
    case AST_ANNOTATE: {
        TypeIdx ty = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).type);
        return TypeCheck(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).expression, ty);
    }
    case AST_SCOPE: {
        return TypeInfer(env, tree, AST_VALUE(tree, node, AST_SCOPE).start);
    }
    case AST_CONS: {
        TypeInfer(env, tree, AST_VALUE(tree, node, AST_CONS).data);
        return TypeInfer(env, tree, AST_VALUE(tree, node, AST_CONS).next);
    }
    case AST_DEFINE: {
        TypeIdx ty = TypeInfer(env, tree, AST_VALUE(tree, node, AST_DEFINE).body);
        String id = AST_VALUE(tree, AST_VALUE(tree, node, AST_DEFINE).name, AST_IDENT);
        TypeEnv_set(env, id, ty);
        return ty;
    }
    default: return TYPE_MAKE(env, TYPE_UNKNOWN);
    }
}

TypeIdx GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node) {
    switch(AST_TY(tree, node)) {
    case AST_IDENT:
        return TypeEnv_get(env, AST_VALUE(tree, node, AST_IDENT));
    default: return TYPE_MAKE(env, TYPE_UNKNOWN);
    }
}

void RunTypeChecker(ASTList* tree, AST_Idx root) {
    TypeEnv env = TypeEnv_make();

    TypeInfer(&env, tree, root);
    
    TypeEnv_free(&env);
}
