#ifndef MERIDIAN_OPCODES_H
#define MERIDIAN_OPCODES_H

#include "meridian_ast.h"
#include "meridian_common.h"

typedef enum {
    OPCODE_STORE,
    OPCODE_LOAD,

    OPCODE_PUSH,
    OPCODE_POP,

    OPCODE_JUMP,

    OPCODE_APPLY,

    OPCODE_CONST,
} Opcode;

typedef struct {
    enum {
        VALUE_NULL,
        
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
        struct { u64 loc; } VALUE_FN;
    } data;
} Value;

#define VALUE(ty) ((Value){ty, {0}})
#define VALUE_V(ty, value) ((Value){ty, value})
#define VALUE_S(ty, ...) ((Value){ty, .ty = (ty){...}})

typedef struct {
    u8* code;
    u64 len, allocated;

    u8* constantsTable;
    u64 constantsLen, constantsAllocated;
} Program;

Program Program_make(void);
void Program_free(Program *program);

void Program_opcode(Program *program, u8 opcode);
void Program_constant(Program *program, Value value);

void Program_codegen(Program* program, NodeList list, NodeIndex node);

#endif//MERIDIAN_OPCODES_H
