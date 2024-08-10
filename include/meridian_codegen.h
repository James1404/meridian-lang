#ifndef MERIDIAN_CODEGEN_H
#define MERIDIAN_CODEGEN_H

#include "meridian_ast.h"

typedef struct {
    AST* tree;
} Codegen;

Codegen Codegen_init(void);
void Codegen_free(Codegen* gen);

void Codegen_run(Codegen* gen, AST* tree);

#endif//MERIDIAN_CODEGEN_H
