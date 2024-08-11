#include "meridian_types.h"


TypeList TypeList_fromAST(ASTList *tree, AST_Idx root);
void TypeList_free(TypeList *list);

TypeIdx TypeList_alloc(TypeList *list, Type t);


TypeChecker TypeChecker_make(void);
void TypeChecker_free(TypeChecker *checker);

void TypeChecker_run(TypeChecker* checker);
