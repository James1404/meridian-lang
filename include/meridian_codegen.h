#ifndef MERIDIAN_CODEGEN_H
#define MERIDIAN_CODEGEN_H

#include "meridian_ast.h"

typedef struct {
    NodeList* list;
    NodeIndex root;
} Codegen;

Codegen Codegen_init(NodeList* list, NodeIndex root);
void Codegen_free(Codegen* gen);

void Codegen_run(Codegen* gen, Node* tree);

#endif//MERIDIAN_CODEGEN_H
