#include "meridian_args_parser.h"

#include <stdio.h>

void ParseArguments(int argc, char** argv, Argument validArgs[], int validArgsLen) {
    if(argc <= 0) {
        printf("Expected arguments\n");

        return;
    }
    
    if(STR_CMP_RAWS(*argv, "help")) {
        for(int i = 0; i < validArgsLen; i++) {
            Argument arg = validArgs[i];
            printf("%.*s :: %.*s\n",
                   arg.name.len, arg.name.raw,
                   arg.desc.len, arg.desc.raw);
        }

        return;
    }
        
    for(int i = 0; i < validArgsLen; i++) {
        Argument* arg = validArgs + i;

        if(STR_CMP_WITH_RAW(arg->name, *argv)) {
            arg->parser(argc - 1, argv + 1);
            break;
        }
    }
}
