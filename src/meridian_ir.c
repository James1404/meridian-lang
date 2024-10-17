#include "meridian_ir.h"
#include "meridian_ast.h"
#include "meridian_error.h"
#include <stdlib.h>

Program Program_make(void) {
    return (Program) {
        .instructions = NULL,
        .len = 0,
        .allocated = 0,
    };
}

void Program_free(Program *program) {
    free(program->instructions);
}


void Program_push(Program *program, Inst inst) {
    if(!program->instructions) {
        program->allocated = 8;
        program->instructions = malloc(sizeof(*program->instructions) * program->allocated);
    }

    u32 idx = program->len++;

    if(program->len >= program->allocated) {
        program->allocated *= 2;
        program->instructions = realloc(program->instructions, sizeof(*program->instructions) * program->allocated);
    }

    program->instructions[idx] = inst;
}

static void Program_output(Program *program, NodeList *tree, NodeIndex node) {
    switch (NODE_TY(tree, node)) {
    case NODE_NULL: {
        Program_push(program, INST_V(INST_PUSH, VALUE_NULL()));
        break;
    }
    case NODE_INTEGER: {
        Value v = VALUE_INT(NODE_VALUE(tree, node, NODE_INTEGER));
        Program_push(program, INST_V(INST_PUSH, v));
        break;
    }
    case NODE_FLOAT: {
        Value v = VALUE_FLOAT(NODE_VALUE(tree, node, NODE_FLOAT));
        Program_push(program, INST_V(INST_PUSH, v));
        break;
    }
    case NODE_BOOLEAN: {
        Value v = VALUE_BOOL(NODE_VALUE(tree, node, NODE_BOOLEAN));
        Program_push(program, INST_V(INST_PUSH, v));
        break;
    }
    case NODE_STRING: {
        Value v = VALUE_STRING(NODE_VALUE(tree, node, NODE_STRING));
        Program_push(program, INST_V(INST_PUSH, v));
        break;
    }
    case NODE_IDENT: {
        break;
    }
    case NODE_CONS: {
        break;
    }
    case NODE_LIST: {
        break;
    }
    case NODE_SCOPE: {
        break;
    }
    case NODE_APPLICATION: {
        break;
    }
    case NODE_ABSTRACTION: {
        break;
    }
    case NODE_LET: {
        Program_output(program, tree, NODE_VALUE(tree, node, NODE_LET).name);
        Program_output(program, tree, NODE_VALUE(tree, node, NODE_LET).value);
        
        Program_push(program, INST(INST_NEWSCOPE));
        
        Program_push(program, INST(INST_SET));
        
        Program_output(program, tree, NODE_VALUE(tree, node, NODE_LET).in);
        
        Program_push(program, INST(INST_RETURN));
        break;
    }
    case NODE_IF: {
        break;
    }
    case NODE_DEFINE: {
        Program_output(program, tree, NODE_VALUE(tree, node, NODE_DEFINE).name);
        Program_output(program, tree, NODE_VALUE(tree, node, NODE_DEFINE).body);
        
        Program_push(program, INST(INST_SET));
        break;
    }
    case NODE_TYPEDEF: {
        break;
    }
    case NODE_ANNOTATE: {
        break;
    }
    case NODE_APPLICATION_TYPE: {
        break;
    }
    }
}

void Program_codegen(Program *program, NodeList *tree, NodeIndex root) {
    Program_output(program, tree, root);
}

Locals Locals_make(void) {
    return (Locals) {
        .data = NULL,
        .len = 0,
        .allocated = 0,
        .scope = 0,
    };
}

void Locals_free(Locals *locals) {
    free(locals->data);
}

void Locals_set(Locals *locals, String name, Value value) {
    if(!locals->data) {
        locals->len = 0;
        locals->allocated = 8;
        locals->data = malloc(sizeof(*locals->data) * locals->allocated);
    }
    
    if(locals->len >= locals->allocated) {
        locals->allocated *= 2;

        Local* temp = realloc(locals->data, sizeof(Local) * locals->allocated);
        if(temp) {
            locals->data = temp;
        }
        else {
            Meridian_error("%s :: realloc error", "TypeLocals_set");
            return;
        }
    }

    Local* local = locals->data + (locals->len++);
    *local = (Local) {
        .scope = locals->scope,
        .value = value,
        .name = name,
    };
}

Value Locals_get(Locals *locals, String name) {
    for(i64 i = locals->len - 1; i >= 0; i--) {
        Local* entry = locals->data + i;

        if(STR_CMP(entry->name, name)) {
            return entry->value;
        }
    }

    Meridian_error("The symbol '%.*s' could not find within the current scope", name.len, name.raw);
    return VALUE_NULL();
}

void Locals_increment(Locals *locals) {
    locals->scope++;
}

void Locals_decrement(Locals *locals) {
    locals->scope--;

    while(locals->data[locals->len - 1].scope > locals->scope) {
        locals->len--;
    }
}

Stack Stack_make(void) {
    return (Stack) {
        .stack = NULL,
        .top = 0,
        .allocated = 0,
    };
}

void Stack_free(Stack *stack) {
    free(stack->stack);
}

void Stack_push(Stack *stack, Value value) {
    if(!stack->stack) {
        stack->allocated = 8;
        stack->stack = malloc(sizeof(*stack->stack) * stack->allocated);
    }

    u32 idx = stack->top++;

    if(stack->top >= stack->allocated) {
        stack->allocated *= 2;
        stack->stack = realloc(stack->stack, sizeof(*stack->stack) * stack->allocated);
    }

    stack->stack[idx] = value;
}

void Stack_pop(Stack *stack) { stack->top--; }

Value Stack_top(Stack *stack) { return stack->stack[stack->top]; }
Value Stack_getAndPop(Stack *stack) {
    Value top = Stack_top(stack);
    Stack_pop(stack);
    return top;
}

CallStack CallStack_make(void) {
    return (CallStack) {
        .stack = NULL,
        .top = 0,
        .allocated = 0,
    };
}

void CallStack_free(CallStack *stack) {
    free(stack->stack);
}

void CallStack_push(CallStack *stack, usize loc) {
    if(!stack->stack) {
        stack->allocated = 8;
        stack->stack = malloc(sizeof(*stack->stack) * stack->allocated);
    }

    u32 idx = stack->top++;

    if(stack->top >= stack->allocated) {
        stack->allocated *= 2;
        stack->stack = realloc(stack->stack, sizeof(*stack->stack) * stack->allocated);
    }

    stack->stack[idx] = loc;
}

usize CallStack_pop(CallStack *stack) {
    return stack->stack[stack->top--];
}

Evaluator Evaluator_make(Program program) {
    return (Evaluator) {
        .program = program,
        .locals = Locals_make(),
        .stack = Stack_make(),
        .callstack = CallStack_make(),
        .pc = 0,
    };
}

void Evaluator_free(Evaluator *eval) {
    Locals_free(&eval->locals);
    Stack_free(&eval->stack);
    CallStack_free(&eval->callstack);
}

void Evaluator_run(Evaluator* eval) {
    while(eval->pc < eval->program.len) {
        Inst instruction = eval->program.instructions[eval->pc++];

        switch (instruction.tag) {
        case INST_ADD: {
            Value rhs = Stack_getAndPop(&eval->stack);
            Value lhs = Stack_getAndPop(&eval->stack);

            if(VALUE_HAS_TAG(lhs, VALUE_INT) &&
               VALUE_HAS_TAG(rhs, VALUE_INT)) {
                Value final = VALUE_INT(VALUE_GET(lhs, VALUE_INT) + VALUE_GET(rhs, VALUE_INT));
                Stack_push(&eval->stack,
                           final);
                continue;
            }

            
            if(VALUE_HAS_TAG(lhs, VALUE_FLOAT) &&
               VALUE_HAS_TAG(rhs, VALUE_FLOAT)) {
                Stack_push(&eval->stack,
                           VALUE(VALUE_FLOAT,
                                 (VALUE_GET(lhs, VALUE_FLOAT) + VALUE_GET(rhs, VALUE_FLOAT))));
                continue;
            }
            
            Stack_push(&eval->stack, VALUE_NULL());
            break;
        }
        case INST_SUB: {
            break;
        }
        case INST_DIV: {
            break;
        }
        case INST_MUL: {
            break;
        }

        case INST_LESS: {
            break;
        }
        case INST_GREATER: {
            break;
        }

        case INST_EQUAL: {
            break;
        }

        case INST_NOT: {
            break;
        }
        case INST_NEGATE: {
            break;
        }

        case INST_NEWSCOPE: {
            CallStack_push(&eval->callstack, eval->pc);
            break;
        }

        case INST_CALL: {
            break;
        }
        case INST_RETURN: {
            break;
        }

        case INST_JUMP: {
            break;
        }
        case INST_JUMP_IF: {
            break;
        }

        case INST_PUSH: {
            Stack_push(&eval->stack, instruction.data.INST_PUSH);
            break;
        }
        case INST_POP: {
            Stack_pop(&eval->stack);
            break;
        }

        case INST_SET: {
            break;
        }
        case INST_GET: {
            break;
        }
        }
    }
}
