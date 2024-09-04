#ifndef MERIDIAN_ARGS_PARSER_H
#define MERIDIAN_ARGS_PARSER_H

#include "meridian_string.h"

typedef void (*ArgumentParser)(int argc, char **argv);

typedef struct {
    String name, desc;
    ArgumentParser parser;
} Argument;

#define MAKE_ARGUMENT(name, desc, parser)                                      \
  ((Argument){STR(name), STR(desc), parser})

void ParseArguments(int argc, char** argv, Argument validArgs[], int validArgsLen);

#endif//MERIDIAN_ARGS_PARSER_H
