#include "meridian.h"
#include "meridian_error.h"
#include "meridian_lex.h"
#include <stdio.h>
#include <stdlib.h>

void Meridian_init(void) {}

void Meridian_free(void) {}

void Meridian_builtin(void) {}

void Meridian_run(char *src, u64 len) {
    Lexer lexer = Lexer_new((String) { src, len });

    Lexer_run(&lexer);

    Lexer_print(&lexer);

    Lexer_free(&lexer);
}

void Meridian_run_file(const char *filepath) {
    FILE* file = fopen(filepath, "r");

    if(!file) {
        Meridian_error("Couldnt not find file");
        return;
    }

    fseek(file, 0, SEEK_END);
    u64 len = ftell(file);

    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(len);

    fread(buffer, 1, len, file);
    fclose(file);

    Meridian_run(buffer, len); 
}
