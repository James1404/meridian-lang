#ifndef MERIDIAN_H
#define MERIDIAN_H

#include "meridian_common.h"

void Meridian_init(void);
void Meridian_free(void);
void Meridian_builtin(void);

void Meridian_run(char *src, u64 len);
void Meridian_run_file(const char* filepath);

#endif//MERIDIAN_H
