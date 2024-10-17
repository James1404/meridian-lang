#ifndef MERIDIAN_IR_H
#define MERIDIAN_IR_H

#include "meridian_ast.h"
#include "meridian_common.h"
#include "meridian_string.h"
#include "meridian_types.h"

//
// Value
//

typedef enum {
    VALUE_NULL,

    VALUE_IDENT,
    VALUE_LOC,
        
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_STRING,
        
    VALUE_FN,
    VALUE_TYPE,
} ValueType;

typedef struct {
    ValueType type;
    union {
        String VALUE_IDENT;
        u64 VALUE_LOC;

        int VALUE_INT;
        float VALUE_FLOAT;
        bool VALUE_BOOL;
        String VALUE_STRING;

        TypeIndex VALUE_TYPE;
    } data;
} Value;

#define VALUE(type, value) ((Value){type, .data.type = value})
#define VALUE_GET(value, type) ((value).data.type)

#define VALUE_HAS_TAG(value, tag) ((value).type == (tag))

#define VALUE_NULL() ((Value){VALUE_NULL, .data = {0}})

#define VALUE_IDENT(v) (VALUE(VALUE_IDENT, (v)))
#define VALUE_LOC(v) (VALUE(VALUE_LOC, (v)))

#define VALUE_INT(v) (VALUE(VALUE_INT, (v)))
#define VALUE_FLOAT(v) (VALUE(VALUE_FLOAT, (v)))
#define VALUE_BOOL(v) (VALUE(VALUE_BOOL, (v)))
#define VALUE_STRING(v) (VALUE(VALUE_STRING, (v)))

#define VALUE_FN(v) (VALUE(VALUE_FN, (v)))
#define VALUE_TYPE(v) (VALUE(VALUE_TYPE, (v)))

//
// Instructions
//

typedef enum {
    INST_ADD,
    INST_SUB,
    INST_DIV,
    INST_MUL,

    INST_LESS,
    INST_GREATER,

    INST_EQUAL,

    INST_NOT,
    INST_NEGATE,

    INST_NEWSCOPE,

    INST_CALL,
    INST_RETURN,

    INST_JUMP,
    INST_JUMP_IF,

    INST_PUSH,
    INST_POP,

    INST_SET,
    INST_GET,
} InstTag;

typedef struct {
    InstTag tag;
    union {
        Value INST_PUSH;
    } data;
} Inst;

#define INST(tag) ((Inst){tag, {0}})
#define INST_V(tag, v) ((Inst){tag, .data.tag = v})

typedef struct {
    Inst* instructions;
    usize len, allocated;
} Program;

Program Program_make(void);
void Program_free(Program *program);

void Program_push(Program *program, Inst inst);

void Program_codegen(Program *program, NodeList *tree, NodeIndex root);

//
// Local Stack
//

typedef struct {
    String name;
    Value value;
    usize scope;
} Local;

typedef struct {
    Local* data;
    usize len, allocated;

    usize scope;
} Locals;

Locals Locals_make(void);
void Locals_free(Locals *locals);

void Locals_set(Locals *locals, String name, Value value);
Value Locals_get(Locals *locals, String name);

void Locals_increment(Locals *locals);
void Locals_decrement(Locals *locals);

//
// Stack
//

typedef struct {
    Value* stack;
    usize top, allocated;
} Stack;

Stack Stack_make(void);
void Stack_free(Stack *stack);

void Stack_push(Stack *stack, Value value);
void Stack_pop(Stack *stack);

Value Stack_top(Stack *stack);
Value Stack_getAndPop(Stack *stack);

//
// Call Stack
//
typedef struct {
    usize* stack;
    usize top, allocated;
} CallStack;

CallStack CallStack_make(void);
void CallStack_free(CallStack *stack);

void CallStack_push(CallStack *stack, usize loc);
usize CallStack_pop(CallStack *stack);

//
// Evaluator
//

typedef struct {
    Program program;
    Locals locals;
    Stack stack;
    CallStack callstack;
    usize pc;
} Evaluator;

Evaluator Evaluator_make(Program program);
void Evaluator_free(Evaluator *eval);

void Evaluator_run(Evaluator *eval);

#endif // MERIDIAN_IR_H
