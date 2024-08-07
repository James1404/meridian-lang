#include "meridian_error.h"

#include <stdio.h>
#include <stdarg.h>

void Meridian_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("Meridian Error :: ");

    vprintf(format, args);

    printf("\n");

    va_end(args);
}
