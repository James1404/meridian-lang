#include "meridian_vm.h"

#include "meridian_error.h"
#include "meridian_string.h"

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
    while(vm->locals[vm->localsLen - 1].scope > vm->localsScope) {
        vm->localsLen--;
    }
}

void VM_set(VM* vm, String name, Value value) {
    if(!vm->locals) {
        vm->localsLen = 0;
        vm->localsAllocated = 8;
        vm->locals = malloc(sizeof(*vm->locals) * vm->localsAllocated);
    }
    
    if(vm->localsLen >= vm->localsAllocated) {
        vm->localsAllocated *= 2;

        Local* temp = realloc(vm->locals, sizeof(Local) * vm->localsAllocated);
        if(temp) {
            vm->locals = temp;
        }
        else {
            Meridian_error("Env_set realloc error");
            return;
        }
    }

    Local* local = vm->locals + (vm->localsLen++);
    *local = (Local) {
        .scope = vm->localsScope,
        .value = value,
    };
    
    STR_CPY_ALLOC(local->name, name);
}

Value VM_get(VM* vm, String name) {
    for(i64 i = vm->localsLen - 1; i >= 0; i--) {
        Local entry = vm->locals[i];

        if(STR_CMP(entry.name, name)) {
            return entry.value;
        }
    }

    Meridian_error("Could not find symbol in type environment");
    return VALUE(VALUE_NULL);
}

void VM_run(VM* vm) {
}
