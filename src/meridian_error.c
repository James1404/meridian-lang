#include "meridian_error.h"
#include "meridian_ast.h"

#include <stdio.h>
#include <stdarg.h>

static bool found = false;

void Meridian_error(const char *format, ...) {
    found = true;
    
    va_list args;
    va_start(args, format);

    printf("Meridian Error :: ");

    vprintf(format, args);

    printf("\n");

    va_end(args);
}

void Meridian_error_ast(NodeList* list, NodeIndex node, const char *format, ...) {
    va_list args;
    va_start(args, format);

    Token t = NODE_GET(list, node)->token;

    printf("[%u, %u] Meridian Error :: ", t.line, t.lineOffset);

    vprintf(format, args);
    printf("\n");

    va_end(args);
}

bool Meridian_error_found(void) {
    return found;
}
