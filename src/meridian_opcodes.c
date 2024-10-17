#include "meridian_opcodes.h"

Program Program_make(void) {
    return (Program) {
        .code = NULL,
        .len = 0,
        .allocated = 8,

        .constantsTable = NULL,
        .constantsLen = 0,
        .constantsAllocated = 8,
    };
}
void Program_free(Program *program) {
}

void Program_opcode(Program *program, u8 opcode) {
}
void Program_byte(Program *program, u8 byte) {
}

void Program_codegen(Program* program, NodeList list, NodeIndex node) {
}
