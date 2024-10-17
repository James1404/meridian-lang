#include "meridian_typechecker.h"

#include "meridian_error.h"
#include "meridian_types.h"

bool Typechecker_isMonoType(TypeChecker *checker, TypeIndex i) {
    switch(TYPE_TAG(checker->allocator, i)) {
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

bool TypeChecker_isPolyType(TypeChecker *checker, TypeIndex i) {
    switch(TYPE_TAG(checker->allocator, i)) {
    case TYPE_FORALL:
        return true;
    default:
        return TypeChecker_isMonoType(checker, i);
    }
}

static bool IsSubtype(TypeChecker *checker, TypeIndex lhs, TypeIndex rhs) {
#define SAME(tag) (TYPE_TAG(checker->allocator, lhs) == (tag) && TYPE_TAG(checker->allocator, rhs) == (tag))
    if(SAME(TYPE_UNIT)) {
        return true;
    }

    if(SAME(TYPE_EXISTS)) {
        return true;
    }

    if(SAME(TYPE_FN)) {
        return 
            IsSubtype(checker, TYPE_VALUE(checker->allocator, lhs, TYPE_FN).arg, TYPE_VALUE(checker->allocator, rhs, TYPE_FN).arg) &&
            IsSubtype(checker, TYPE_VALUE(checker->allocator, lhs, TYPE_FN).body, TYPE_VALUE(checker->allocator, rhs, TYPE_FN).body);
    }

    if(TYPE_TAG(checker->allocator, lhs) == TYPE_EXISTS) {
        if(!TypeChecker_occursCheck(checker, lhs, rhs)) {
            TYPE_SET(checker->allocator, lhs, TypeChecker_instantiateL(checker, rhs));
            return IsSubtype(checker, lhs, rhs);
        }
    }

    if(TYPE_TAG(checker->allocator, rhs) == TYPE_EXISTS) {
        if(!TypeChecker_occursCheck(checker, lhs, rhs)) {
            TYPE_SET(checker->allocator, rhs, TypeChecker_instantiateR(checker, lhs));
            return IsSubtype(checker, lhs, rhs);
        }
    }

    return TYPE_TAG(checker->allocator, lhs) == TYPE_TAG(checker->allocator, rhs);
#undef SAME
}

String TypeChecker_toString(TypeChecker *checker, TypeIndex ty) {
    switch(TYPE_TAG(checker->allocator, ty)) {
    case TYPE_NOT_FOUND: return STR("Not Found");
                
    case TYPE_UNKNOWN: return STR("Unknown");

    case TYPE_UNIT: return STR("Unit");
    case TYPE_INT: return STR("Int");
    case TYPE_FLOAT: return STR("Float");
    case TYPE_BOOLEAN: return STR("Bool");
    case TYPE_STRING: return STR("String");

    case TYPE_FN: {
        String arg = TypeChecker_toString(checker, TYPE_VALUE(checker->allocator, ty, TYPE_FN).arg);
        String body = TypeChecker_toString(checker, TYPE_VALUE(checker->allocator, ty, TYPE_FN).body);
        return Arena_fmt(&checker->arena, "%.*s -> %.*s", arg.len, arg.raw, body.len, body.raw);
    }

    case TYPE_FORALL: return STR("forall");
    case TYPE_EXISTS: return STR("exists");
    }

    return STR("TypeError");
}

bool TypeChecker_isWellFormed(TypeChecker *checker, TypeIndex ty) {
    switch(TYPE_TAG(checker->allocator, ty)) {
    case TYPE_FN:
        return
            TypeChecker_isWellFormed(checker, TYPE_VALUE(checker->allocator, ty, TYPE_FN).arg) &&
            TypeChecker_isWellFormed(checker, TYPE_VALUE(checker->allocator, ty, TYPE_FN).body);
    default:
        return true;
    }
}

bool TypeChecker_occursCheck(TypeChecker *checker, TypeIndex a, TypeIndex against) {
    return false;
}

TypeIndex TypeChecker_instantiateL(TypeChecker *checker, TypeIndex mono) {
    // InstLSolve
    if(TypeChecker_isMonoType(checker, mono) && TypeChecker_isWellFormed(checker, mono)) {
        return mono;
    }
    //InstLArr
    if(TYPE_TAG(checker->allocator, mono) == TYPE_FN) {
        TypeIndex ex1 = TYPE_MAKE(checker->allocator, TYPE_EXISTS);
        TypeIndex ex2 = TYPE_MAKE(checker->allocator, TYPE_EXISTS);

        ex1 = TypeChecker_instantiateR(checker, TYPE_VALUE(checker->allocator, mono, TYPE_FN).arg);
        ex2 = TypeChecker_instantiateL(checker, TYPE_VALUE(checker->allocator, mono, TYPE_FN).body);

        return TYPE_MAKE_S(checker->allocator, TYPE_FN, ex1, ex2);
    }

    //InstLALLR
    if(TYPE_TAG(checker->allocator, mono) == TYPE_FORALL) {
        return mono;
    }
    
    //InstLReach
    if(TYPE_TAG(checker->allocator, mono) == TYPE_EXISTS) {
        return mono;
    }

    return TYPE_MAKE(checker->allocator, TYPE_EXISTS);
}

TypeIndex TypeChecker_instantiateR(TypeChecker *checker, TypeIndex mono) {
    // InstRSolve
    if(TypeChecker_isMonoType(checker, mono) && TypeChecker_isWellFormed(checker, mono)) {
        return mono;
    }
    
    //InsRLArr
    if(TYPE_TAG(checker->allocator, mono) == TYPE_FN) {
        TypeIndex ex1 = TYPE_MAKE(checker->allocator, TYPE_EXISTS);
        TypeIndex ex2 = TYPE_MAKE(checker->allocator, TYPE_EXISTS);

        ex1 = TypeChecker_instantiateL(checker, TYPE_VALUE(checker->allocator, mono, TYPE_FN).arg);
        ex2 = TypeChecker_instantiateR(checker, TYPE_VALUE(checker->allocator, mono, TYPE_FN).body);

        return TYPE_MAKE_S(checker->allocator, TYPE_FN, ex1, ex2);
    }
    
    //InstRALLR
    if(TYPE_TAG(checker->allocator, mono) == TYPE_FORALL) {
        return mono;
    }
    
    //InstRReach
    if(TYPE_TAG(checker->allocator, mono) == TYPE_EXISTS) {
        return mono;
    }

    return TYPE_MAKE(checker->allocator, TYPE_EXISTS);
}

bool TypeCheck(TypeChecker *checker, NodeIndex node, TypeIndex expected) {
    switch(NODE_TY(&checker->env->nodes, node)) {
    case NODE_IDENT: {
        TypeIndex inferred = TypeInfer(checker, node);

        if(!IsSubtype(checker, inferred, expected)) {
            String inferredstr = TypeChecker_toString(checker, inferred);
            String expectedstr = TypeChecker_toString(checker, expected);
            Meridian_error_ast(&checker->env->nodes, node, "Expected type of '%.*s', but got '%.*s'", expectedstr.len, expectedstr.raw, inferredstr.len, inferredstr.raw);

            return false;
        }
        
        break;
    }
    case NODE_ANNOTATE: {
        TypeIndex t = GetTypeFromAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ANNOTATE).type);
        TypeCheck(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ANNOTATE).expression, t);
        break;
    }
    default: {
        TypeIndex inferred = TypeInfer(checker, node);

        if(!IsSubtype(checker, inferred, expected)) {
            String inferredstr = TypeChecker_toString(checker, inferred);
            String expectedstr = TypeChecker_toString(checker, expected);
            Meridian_error_ast(&checker->env->nodes, node, "Expression of type '%.*s' does not match the annotation '%.*s'", inferredstr.len, inferredstr.raw, expectedstr.len, expectedstr.raw);

            return false;
        }
        break;
    }
    }

    return true;
}

TypeIndex TypeInfer(TypeChecker *checker, NodeIndex node) {
    switch(NODE_TY(&checker->env->nodes, node)) {
    case NODE_INTEGER: return TYPE_MAKE(checker->allocator, TYPE_INT);
    case NODE_FLOAT: return TYPE_MAKE(checker->allocator, TYPE_FLOAT);
    case NODE_BOOLEAN: return TYPE_MAKE(checker->allocator, TYPE_BOOLEAN);
    case NODE_STRING: return TYPE_MAKE(checker->allocator, TYPE_STRING);
    case NODE_IDENT: {
        String id = NODE_VALUE(&checker->env->nodes, node, NODE_IDENT);
        TypeIndex t = TypeChecker_get(checker, id)->ty;

        if(TYPE_TAG(checker->allocator, t) == TYPE_NOT_FOUND) {
            Meridian_error_ast(&checker->env->nodes, node, "Type identifier does not exist '%.*s'", id.len, id.raw);
        }

        return t;
    }
    case NODE_ANNOTATE: {
        TypeIndex ty = GetTypeFromAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ANNOTATE).type);
        TypeCheck(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ANNOTATE).expression, ty);

        return ty;
    }
    case NODE_SCOPE: {
        return TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_SCOPE));
    }
    case NODE_CONS: {
        TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_CONS).data);
        return TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_CONS).next);
    }
    case NODE_LET: {
        TypeChecker_increment(checker);
                
        TypeIndex letTy = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_LET).value);

        TypeChecker_insertAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_LET).name, letTy);

        TypeIndex inTy = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_LET).in);

        TypeChecker_decrement(checker);

        return inTy;
    }
    case NODE_IF: {
        TypeCheck(checker, NODE_VALUE(&checker->env->nodes, node, NODE_IF).cond, TYPE_MAKE(checker->allocator, TYPE_BOOLEAN));

        TypeIndex t = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_IF).t);
        TypeIndex f = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_IF).f);

        IsSubtype(checker, t, f);

        return t;
    }

    case NODE_DEFINE: {
        TypeIndex ty = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_DEFINE).body);
        String id = NODE_VALUE(&checker->env->nodes, NODE_VALUE(&checker->env->nodes, node, NODE_DEFINE).name, NODE_IDENT);
        TypeChecker_set(checker, id, node, ty);
        return ty;
    }
    case NODE_ABSTRACTION: {
        TypeIndex arg = TYPE_MAKE(checker->allocator, TYPE_EXISTS);
        TypeChecker_insertAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ABSTRACTION).arg, arg);

        
        TypeIndex body = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_ABSTRACTION).body);
        return TYPE_MAKE_S(checker->allocator, TYPE_FN, arg, body);
    }
    case NODE_APPLICATION: {
        TypeIndex lambda = TypeInfer(checker, NODE_VALUE(&checker->env->nodes, node, NODE_APPLICATION).fn);

        if(TYPE_TAG(checker->allocator, lambda) != TYPE_FN) {
            String lambdaStr = TypeChecker_toString(checker, lambda);
            Meridian_error_ast(&checker->env->nodes, node, "Cannot apply an expression to type '%.*s'", lambdaStr.len, lambdaStr.raw);
        }

        TypeCheck(checker, NODE_VALUE(&checker->env->nodes, node, NODE_APPLICATION).arg, TYPE_VALUE(checker->allocator, lambda, TYPE_FN).arg);

        return TYPE_VALUE(checker->allocator, lambda, TYPE_FN).body;
    }
    default: return TYPE_MAKE(checker->allocator, TYPE_UNKNOWN);
    }
}

void TypeChecker_insertAST(TypeChecker *checker, NodeIndex node, TypeIndex type) {
    switch(NODE_TY(&checker->env->nodes, node)) {
    case NODE_IDENT:
        TypeChecker_set(checker, NODE_VALUE(&checker->env->nodes, node, NODE_IDENT), node, type);
        break;
    default:
        break;
    }  
}

TypeIndex GetTypeFromAST(TypeChecker *checker, NodeIndex node) {
    switch(NODE_TY(&checker->env->nodes, node)) {
    case NODE_IDENT:
        String name = NODE_VALUE(&checker->env->nodes, node, NODE_IDENT);
        TypeIndex idx = TypeAllocator_getTypeName(checker->allocator, name);
        if(TYPE_TAG(checker->allocator, idx) != TYPE_NOT_FOUND) {
            return idx;
        }

        return TypeChecker_get(checker, name)->ty;
    case NODE_APPLICATION_TYPE: {
        TypeIndex arg = GetTypeFromAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_APPLICATION_TYPE).arg);
        TypeIndex ret = GetTypeFromAST(checker, NODE_VALUE(&checker->env->nodes, node, NODE_APPLICATION_TYPE).fn);

        return TYPE_MAKE_S(checker->allocator, TYPE_FN, arg, ret);
    }
    default: return TYPE_MAKE(checker->allocator, TYPE_UNKNOWN);
    }
}

void RunTypeChecker(TypeChecker *checker, NodeIndex root) {
    TypeInfer(checker, root);
}
