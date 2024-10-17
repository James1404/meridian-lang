#ifndef MERIDIAN_TYPECHECKER_H
#define MERIDIAN_TYPECHECKER_H

#include "meridian_arena.h"
#include "meridian_types.h"
#include "meridian_env.h"
#include "meridian_ast.h"

typedef struct {
    NodeList* nodes;
    Env* env;
    TypeAllocator* allocator;
    Arena arena;
} TypeChecker;

TypeChecker TypeChecker_make(Env *env);
void TypeChecker_free(Env *env);

bool TypeChecker_isMonoType(TypeChecker *checker, TypeIndex i);
bool TypeChecker_isPolyType(TypeChecker *checker, TypeIndex i);

void TypeChecker_insertAST(TypeChecker *checker, NodeIndex node, TypeIndex type);
TypeIndex GetTypeFromAST(TypeChecker *checker, NodeIndex node);

bool TypeCheck(TypeChecker *checker, NodeIndex node, TypeIndex expected);
TypeIndex TypeInfer(TypeChecker *checker, NodeIndex node);

bool TypeChecker_occursCheck(TypeChecker *checker, TypeIndex a, TypeIndex against);

TypeIndex TypeChecker_instantiateL(TypeChecker *checker, TypeIndex mono);
TypeIndex TypeChecker_instantiateR(TypeChecker *checker, TypeIndex mono);

String TypeChecker_toString(TypeChecker *checker, TypeIndex ty);

void TypeChecker_run(TypeChecker *checker, NodeIndex root);

#endif//MERIDIAN_TYPECHECKER_H
