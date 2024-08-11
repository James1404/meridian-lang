#ifndef MERIDIAN_OPCODES_H
#define MERIDIAN_OPCODES_H

#include "meridian_common.h"

typedef struct {
    enum {
        OPCODE_STORE,
        OPCODE_LOAD,

        OPCODE_PUSH,
        OPCODE_POP,

        OPCODE_JUMP,

        OPCODE_APPLY,

        OPCODE_CONST,
    } ty;

    union {
        u64 OPCODE_JUMP;
    } payload;
} Op;

typedef struct {
    Op* code;
    u64 len, allocated;
} Program;

#endif//MERIDIAN_OPCODES_H
