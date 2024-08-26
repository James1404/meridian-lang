#include "meridian_types.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include "meridian_string.h"

#include <stdio.h>
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

        .universalQuantifiers = 0,

        .complete = false,

        .arena = Arena_make(),
    };
}

void TypeEnv_free(TypeEnv *env) {
    free(env->locals);
    free(env->typelist);

    Arena_free(&env->arena);
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

bool TypeEnv_isMonoType(TypeEnv *env, TypeIdx i) {
    switch(TYPE_TAG(env, i)) {
    case TYPE_UNIT:
    case TYPE_INT:
    case TYPE_FLOAT:
    case TYPE_BOOLEAN:
    case TYPE_STRING:
    case TYPE_FN:
        return true;
    default:
        return false;
    }
}

bool TypeEnv_isPolyType(TypeEnv *env, TypeIdx i) {
    switch(TYPE_TAG(env, i)) {
    case TYPE_FORALL:
        return true;
    default:
        return TypeEnv_isMonoType(env, i);
    }
}

bool IsSubtype(TypeEnv* env, TypeIdx lhs, TypeIdx rhs) {
#define SAME(tag) (TYPE_TAG(env, lhs) == (tag) && TYPE_TAG(env, rhs) == (tag))
    if(SAME(TYPE_UNIT)) {
        return true;
    }

    if(SAME(TYPE_EXISTS)) {
        return true;
    }

    if(SAME(TYPE_FN)) {
        return 
            IsSubtype(env, TYPE_VALUE(env, lhs, TYPE_FN).arg, TYPE_VALUE(env, rhs, TYPE_FN).arg) &&
            IsSubtype(env, TYPE_VALUE(env, lhs, TYPE_FN).body, TYPE_VALUE(env, rhs, TYPE_FN).body);
    }

    if(TYPE_TAG(env, lhs) == TYPE_EXISTS) {
        if(!TypeEnv_occursCheck(env, lhs, rhs)) {
            TYPE_SET(env, lhs, TypeEnv_instantiateL(env, rhs));
            return IsSubtype(env, lhs, rhs);
        }
    }

    if(TYPE_TAG(env, rhs) == TYPE_EXISTS) {
        if(!TypeEnv_occursCheck(env, lhs, rhs)) {
            TYPE_SET(env, rhs, TypeEnv_instantiateR(env, lhs));
            return IsSubtype(env, lhs, rhs);
        }
    }

    return TYPE_TAG(env, lhs) == TYPE_TAG(env, rhs);
#undef SAME
}

String TypeEnv_toString(TypeEnv* env, TypeIdx ty) {
    switch(TYPE_TAG(env, ty)) {
    case TYPE_NOT_FOUND: return STR("Not Found");
                
    case TYPE_UNKNOWN: return STR("Unknown");

    case TYPE_UNIT: return STR("Unit");
    case TYPE_INT: return STR("Int");
    case TYPE_FLOAT: return STR("Float");
    case TYPE_BOOLEAN: return STR("Bool");
    case TYPE_STRING: return STR("String");

    case TYPE_FN: {
        String arg = TypeEnv_toString(env, TYPE_VALUE(env, ty, TYPE_FN).arg);
        String body = TypeEnv_toString(env, TYPE_VALUE(env, ty, TYPE_FN).body);
        return Arena_fmt(&env->arena, "%.*s -> %.*s", arg.len, arg.raw, body.len, body.raw);
    }

    case TYPE_FORALL: return STR("forall");
    case TYPE_EXISTS: return STR("exists");
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

bool TypeEnv_isWellFormed(TypeEnv* env, TypeIdx ty) {
    switch(TYPE_TAG(env, ty)) {
    case TYPE_FN:
        return
            TypeEnv_isWellFormed(env, TYPE_VALUE(env, ty, TYPE_FN).arg) &&
            TypeEnv_isWellFormed(env, TYPE_VALUE(env, ty, TYPE_FN).body);
    default:
        return true;
    }
}

TypeIdx TypeEnv_get(TypeEnv* env, String name) {
    TypeIdx ty = TypeEnv_GetTypeName(env, name);
    if(TYPE_TAG(env, ty) != TYPE_NOT_FOUND) return ty;
    
    for(i64 i = env->len - 1; i >= 0; i--) {
        LocalType entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.ty;
        }
    }

    Meridian_error("The symbol '%.*s' could not be found in the current scope", name.len, name.raw);
    return TYPE_MAKE(env, TYPE_NOT_FOUND);
}

bool TypeEnv_has(TypeEnv* env, String name) {
    TypeIdx ty = TypeEnv_GetTypeName(env, name);
    if(TYPE_TAG(env, ty) != TYPE_NOT_FOUND) return false;
    
    for(i64 i = env->len - 1; i >= 0; i--) {
        LocalType entry = env->locals[i];

        if(STR_CMP(entry.name, name)) {
            return true;
        }
    }

    return false;
}

bool TypeEnv_occursCheck(TypeEnv* env, TypeIdx a, TypeIdx against) {
    return false;
}

TypeIdx TypeEnv_instantiateL(TypeEnv *env, TypeIdx mono) {
    // InstLSolve
    if(TypeEnv_isMonoType(env, mono) && TypeEnv_isWellFormed(env, mono)) {
        return mono;
    }
    //InstLArr
    if(TYPE_TAG(env, mono) == TYPE_FN) {
        TypeIdx ex1 = TYPE_MAKE(env, TYPE_EXISTS);
        TypeIdx ex2 = TYPE_MAKE(env, TYPE_EXISTS);

        ex1 = TypeEnv_instantiateR(env, TYPE_VALUE(env, mono, TYPE_FN).arg);
        ex2 = TypeEnv_instantiateL(env, TYPE_VALUE(env, mono, TYPE_FN).body);

        return TYPE_MAKE_S(env, TYPE_FN, ex1, ex2);
    }

    //InstLALLR
    if(TYPE_TAG(env, mono) == TYPE_FORALL) {
        return mono;
    }
    
    //InstLReach
    if(TYPE_TAG(env, mono) == TYPE_EXISTS) {
        return mono;
    }

    return TYPE_MAKE(env, TYPE_EXISTS);
}

TypeIdx TypeEnv_instantiateR(TypeEnv *env, TypeIdx mono) {
    // InstRSolve
    if(TypeEnv_isMonoType(env, mono) && TypeEnv_isWellFormed(env, mono)) {
        return mono;
    }
    
    //InsRLArr
    if(TYPE_TAG(env, mono) == TYPE_FN) {
        TypeIdx ex1 = TYPE_MAKE(env, TYPE_EXISTS);
        TypeIdx ex2 = TYPE_MAKE(env, TYPE_EXISTS);

        ex1 = TypeEnv_instantiateL(env, TYPE_VALUE(env, mono, TYPE_FN).arg);
        ex2 = TypeEnv_instantiateR(env, TYPE_VALUE(env, mono, TYPE_FN).body);

        return TYPE_MAKE_S(env, TYPE_FN, ex1, ex2);
    }
    
    //InstRALLR
    if(TYPE_TAG(env, mono) == TYPE_FORALL) {
        return mono;
    }
    
    //InstRReach
    if(TYPE_TAG(env, mono) == TYPE_EXISTS) {
        return mono;
    }

    return TYPE_MAKE(env, TYPE_EXISTS);
}

bool TypeCheck(TypeEnv* env, ASTList* tree, AST_Idx node, TypeIdx expected) {
    switch(AST_TY(tree, node)) {
    case AST_IDENT: {
        TypeIdx inferred = TypeInfer(env, tree, node);

        if(!IsSubtype(env, inferred, expected)) {
            String inferredstr = TypeEnv_toString(env, inferred);
            String expectedstr = TypeEnv_toString(env, expected);
            Meridian_error_ast(tree, node, "Identifier's type '%.*s' is not equal to the expected type '%.*s'", inferredstr.len, inferredstr.raw, expectedstr.len, expectedstr.raw);

            return false;
        }
        
        break;
    }
    case AST_ANNOTATE: {
        TypeIdx t = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).type);
        TypeCheck(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).expression, t);
        break;
    }
    default: {
        TypeIdx inferred = TypeInfer(env, tree, node);

        if(!IsSubtype(env, inferred, expected)) {
            String inferredstr = TypeEnv_toString(env, inferred);
            String expectedstr = TypeEnv_toString(env, expected);
            Meridian_error_ast(tree, node, "Expression of type '%.*s' does not match the annotation '%.*s'", inferredstr.len, inferredstr.raw, expectedstr.len, expectedstr.raw);

            return false;
        }
        break;
    }
    }

    return true;
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

        if(TYPE_TAG(env, t) == TYPE_NOT_FOUND) {
            Meridian_error_ast(tree, node, "Type identifier does not exist '%.*s'", id.len, id.raw);
        }

        return t;
    }
    case AST_ANNOTATE: {
        TypeIdx ty = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).type);
        TypeCheck(env, tree, AST_VALUE(tree, node, AST_ANNOTATE).expression, ty);

        return ty;
    }
    case AST_SCOPE: {
        return TypeInfer(env, tree, AST_VALUE(tree, node, AST_SCOPE).start);
    }
    case AST_CONS: {
        TypeInfer(env, tree, AST_VALUE(tree, node, AST_CONS).data);
        return TypeInfer(env, tree, AST_VALUE(tree, node, AST_CONS).next);
    }
    case AST_LET: {
        TypeEnv_inc(env);
                
        TypeIdx letTy = TypeInfer(env, tree, AST_VALUE(tree, node, AST_LET).value);

        TypeEnv_insertAST(env, tree, AST_VALUE(tree, node, AST_LET).name, letTy);

        TypeIdx inTy = TypeInfer(env, tree, AST_VALUE(tree, node, AST_LET).in);

        TypeEnv_dec(env);

        return inTy;
    }
    case AST_IF: {
        TypeCheck(env, tree, AST_VALUE(tree, node, AST_IF).cond, TYPE_MAKE(env, TYPE_BOOLEAN));

        TypeIdx t = TypeInfer(env, tree, AST_VALUE(tree, node, AST_IF).t);
        TypeIdx f = TypeInfer(env, tree, AST_VALUE(tree, node, AST_IF).f);

        IsSubtype(env, t, f);

        return t;
    }

    case AST_DEFINE: {
        TypeIdx ty = TypeInfer(env, tree, AST_VALUE(tree, node, AST_DEFINE).body);
        String id = AST_VALUE(tree, AST_VALUE(tree, node, AST_DEFINE).name, AST_IDENT);
        TypeEnv_set(env, id, ty);
        return ty;
    }
    case AST_ABSTRACTION: {
        TypeIdx arg = TYPE_MAKE(env, TYPE_EXISTS);
        TypeEnv_insertAST(env, tree, AST_VALUE(tree, node, AST_ABSTRACTION).arg, arg);

        
        TypeIdx body = TypeInfer(env, tree, AST_VALUE(tree, node, AST_ABSTRACTION).body);
        return TYPE_MAKE_S(env, TYPE_FN, arg, body);
    }
    case AST_APPLICATION: {
        TypeIdx lambda = TypeInfer(env, tree, AST_VALUE(tree, node, AST_APPLICATION).fn);

        if(TYPE_TAG(env, lambda) != TYPE_FN) {
            String lambdaStr = TypeEnv_toString(env, lambda);
            Meridian_error_ast(tree, node, "Cannot apply an expression to type '%.*s'", lambdaStr.len, lambdaStr.raw);
        }

        TypeCheck(env, tree, AST_VALUE(tree, node, AST_APPLICATION).arg, TYPE_VALUE(env, lambda, TYPE_FN).arg);

        return TYPE_VALUE(env, lambda, TYPE_FN).body;
    }
    default: return TYPE_MAKE(env, TYPE_UNKNOWN);
    }
}

void TypeEnv_insertAST(TypeEnv *env, ASTList *tree, AST_Idx node, TypeIdx type) {
    switch(AST_TY(tree, node)) {
    case AST_IDENT:
        TypeEnv_set(env, AST_VALUE(tree, node, AST_IDENT), type);
        break;
    default:
        break;
    }  
}

TypeIdx GetTypeFromAST(TypeEnv *env, ASTList *tree, AST_Idx node) {
    switch(AST_TY(tree, node)) {
    case AST_IDENT:
        return TypeEnv_get(env, AST_VALUE(tree, node, AST_IDENT));
    case AST_APPLICATION_TYPE: {
        TypeIdx arg = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_APPLICATION_TYPE).arg);
        TypeIdx ret = GetTypeFromAST(env, tree, AST_VALUE(tree, node, AST_APPLICATION_TYPE).fn);

        return TYPE_MAKE_S(env, TYPE_FN, arg, ret);
    }
    default: return TYPE_MAKE(env, TYPE_UNKNOWN);
    }
}

void RunTypeChecker(ASTList* tree, AST_Idx root) {
    TypeEnv env = TypeEnv_make();

    TypeInfer(&env, tree, root);
    
    TypeEnv_free(&env);
}
