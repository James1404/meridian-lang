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
    String id;
    Value value;
    u64 scope;
} Local;

typedef struct {
    Local* locals;
    u64 len, allocated, scope;
} Env;


typedef struct {
    u64 pc;

    Program code;

    Value* stack;
    u64 stackTop, stackAllocated;

    Local* locals;
    u64 localsLen, localsAllocated;
    u64 localsScope;
} VM;

VM VM_make(Program code);
void VM_free(VM *vm);

void VM_inc(VM *vm);
void VM_dec(VM *vm);

void VM_set(VM* vm, String id, Value value);
Value VM_get(VM* vm, String id);

void VM_run(VM* vm);

#endif//MERIDIAN_VM_H
