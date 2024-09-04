#include "meridian_codegen.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

Codegen Codegen_init(ASTList *list, AST_Idx root) {
    return { list, root };
}

void Codegen_free(Codegen *gen) {}
void Codegen_run(Codegen *gen, AST *tree) {}
