#include "meridian_vm.h"

#include <stdlib.h>

VM VM_make(Program code) {
    return (VM) {
        .pc = 0,
        .code = code,

        .stack = NULL,
        .stackTop = 0,
        .stackAllocated = 8,

        .locals = NULL,
        .localsLen = 0,
        .localsAllocated = 8,
        .localsScope = 0
    };
}

void VM_free(VM *vm) {
    free(vm->stack);
    free(vm->locals);
}

void VM_inc(VM *vm) {
    vm->localsScope++;
}
void VM_dec(VM *vm) {
}

void VM_set(VM* vm, String id, Value value) {
    if(!vm->locals) {
        vm->localsAllocated = 8;
        vm->locals = malloc(sizeof(*vm->locals) * vm->localsAllocated);
    }
}

Value VM_get(VM* vm, String id) {
}

void VM_run(VM* vm) {
}
