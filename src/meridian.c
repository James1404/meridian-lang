#include "meridian.h"
#include "meridian_lex.h"

void Meridian_init(void) {}

void Meridian_free(void) {}

void Meridian_builtin(void) {}

void Meridian_run(char *src, u64 len) {
    Lexer lexer = Lexer_new((String) { src, len });

    Lexer_run(&lexer);

    Lexer_free(&lexer);
}

void Meridian_run_file(const char *filepath) {}

