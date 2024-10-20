#ifndef MERIDIAN_VM_H
#define MERIDIAN_VM_H

#include "meridian_common.h"
#include "meridian_opcodes.h"
#include "meridian_string.h"

typedef struct {
    String name;
    Value value;
    u64 scope;
} VMLocal;

typedef struct {
    VMLocal* locals;
    u64 len, allocated, scope;
} VMEnv;

typedef struct {
    u64 pc;

    Program code;

    Value* stack;
    u64 stackTop, stackAllocated;

    VMLocal* locals;
    u64 localsLen, localsAllocated;
    u64 localsScope;
} VM;

VM VM_make(Program code);
void VM_free(VM *vm);

void VM_inc(VM *vm);
void VM_dec(VM *vm);

void VM_set(VM* vm, String name, Value value);
Value VM_get(VM* vm, String name);

void VM_run(VM* vm);

#endif//MERIDIAN_VM_H
