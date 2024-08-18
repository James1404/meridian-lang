#ifndef MERIDIAN_ERROR_H
#define MERIDIAN_ERROR_H

#include "meridian_ast.h"
#include "meridian_common.h"

void Meridian_error(const char *format, ...);
void Meridian_error_ast(ASTList* list, AST_Idx node, const char *format, ...);

bool Meridian_error_found(void);

#endif//MERIDIAN_ERROR_H
