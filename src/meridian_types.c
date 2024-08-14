#include "meridian_types.h"
#include "meridian_error.h"
#include "meridian_string.h"

#include <stdlib.h>

String Type_toString(Type ty) {
    switch(ty.tag) {
    case TYPE_UNKNOWN: return STR("Unknown");
    case TYPE_ID: return STR("Id");
        
    case TYPE_UNIT: return STR("Unit");
    case TYPE_INT: return STR("Int");
    case TYPE_FLOAT: return STR("Float");
    case TYPE_BOOLEAN: return STR("Bool");
    case TYPE_STRING: return STR("String");
        
    default: return STR("TypeError");
    }
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
    Type ty = GetTypeName(name);
    if(ty.tag != TYPE_UNKNOWN) return ty;
    
    for(i64 i = env->len - 1; i >= 0; i--) {
        LocalType entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.ty;
        }
    }



    Meridian_error("The symbol '%.*s' could not be found in the current scope", name.len, name.raw);
    return (Type) { 0 };
}

Type TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, Type expected) {
    Type inferred = TypeInfer(env, tree, node);

    if(inferred.tag != expected.tag) {
        String inferredstr = Type_toString(inferred);
        String expectedstr = Type_toString(expected);
        Meridian_error("Expression of type '%.*s' does not match the annotation '%.*s'", inferredstr.len, inferredstr.raw, expectedstr.len, expectedstr.raw);
    }

    return inferred;
}

Type TypeInfer(TypeEnv* env, ASTList* tree, AST_Idx node) {
    switch(AST_TY(tree, node)) {
    case AST_INTEGER: return TYPE_MAKE(TYPE_INT);
    case AST_FLOAT: return TYPE_MAKE(TYPE_FLOAT);
    case AST_BOOLEAN: return TYPE_MAKE(TYPE_BOOLEAN);
    case AST_STRING: return TYPE_MAKE(TYPE_STRING);
    case AST_IDENT: {
        String id = AST_VALUE(tree, node, AST_IDENT);
        Type t = TypeEnv_get(env, id);

        if(t.tag == TYPE_UNKNOWN) {
            Meridian_error("Type identifier does not exist '%.*s'", id.len, id.raw);
        }

        return t;
    }
    case AST_ANNOTATE: {
        Type ty = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).type);

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
        Type ty = TypeInfer(env, tree, AST_VALUE(tree, node, AST_DEFINE).body);
        String id = AST_VALUE(tree, AST_VALUE(tree, node, AST_DEFINE).name, AST_IDENT);
        TypeEnv_set(env, id, ty);
        return ty;
    }
    default: return TYPE_MAKE(TYPE_UNKNOWN);
    }
}

Type GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node) {
    switch(AST_TY(tree, node)) {
    case AST_IDENT:
        return TypeEnv_get(env, AST_VALUE(tree, node, AST_IDENT));
    default: return TYPE_MAKE(TYPE_UNKNOWN);
    }
}

void RunTypeChecker(ASTList* tree, AST_Idx root) {
    TypeEnv env = TypeEnv_make();

    TypeInfer(&env, tree, root);
    
    TypeEnv_free(&env);
}
