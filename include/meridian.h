#ifndef MERIDIAN_H
#define MERIDIAN_H

#include "meridian_env.h"
#include "meridian_string.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 01

void Meridian_run(String src, Env* env);
void Meridian_run_file(String filepath, Env* env);

#endif//MERIDIAN_H
