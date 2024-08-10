#ifndef MERIDIAN_VM_H
#define MERIDIAN_VM_H

#include "meridian_common.h"
#include "meridian_opcodes.h"
#include "meridian_string.h"

typedef struct {
    enum {
        VALUE_INT,
        VALUE_FLOAT,
        VALUE_BOOLEAN,
        VALUE_STRING,
        VALUE_IDENT,
        VALUE_FN,
    } ty;
    union {
        i64 VALUE_INT;
        f64 VALUE_FLOAT;
        bool VALUE_BOOLEAN;
        String VALUE_STRING;
        String VALUE_IDENT;
        struct { } VALUE_FN;
    } data;
} Value;

typedef struct {
    u64 pc;

    Program code;

    Value* stack;
    u64 stackTop, stackAllocated;
} VM;

VM VM_make(Program code);
void VM_free(VM *vm);

void VM_run(VM* vm);

#endif//MERIDIAN_VM_H
