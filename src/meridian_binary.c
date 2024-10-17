#include "meridian.h"
#include "meridian_env.h"
#include "meridian_string.h"
#include "meridian_args_parser.h"
#include "meridian_error.h"

#include <stdio.h>

#define REPL_LEN 256

static bool running = true;

void versionParser(int argc, char** argv) {
    printf("%s v%u.%u\n", EXE_NAME, MAJOR_VERSION, MINOR_VERSION);
}

void replParser(int argc, char** argv) {
    Env env = Env_make();
    
    while(running) {
        printf("* ");
        char string[REPL_LEN];
        if(!fgets(string, REPL_LEN, stdin)) {
            Meridian_error("fgets failed");
        }

        Meridian_run(STR(string), &env);
    }

    Env_free(&env);
}

void runParser(int argc, char** argv) {
    Env env = Env_make();
        
    Meridian_run_file(STR("test/lexer.mr"), &env);
    
    Env_free(&env);
}

int main(int argc, char** argv) {
    argc--;
    argv++;
    
    Argument validArguments[] = {
        MAKE_ARGUMENT("version", "output the version of your installed compiler", &versionParser),
        MAKE_ARGUMENT("run", "run the provided file", &runParser),
        MAKE_ARGUMENT("repl", "run the interactive REPL", &replParser),
    };
    int validArgumentsLen = sizeof(validArguments) / sizeof(*validArguments);
    
    ParseArguments(argc, argv, validArguments, validArgumentsLen);
    return 0;
}
