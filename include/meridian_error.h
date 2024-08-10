#ifndef MERIDIAN_ERROR_H
#define MERIDIAN_ERROR_H

#include "meridian_common.h"

void Meridian_error(const char *format, ...);
bool Meridian_error_found(void);

#endif//MERIDIAN_ERROR_H
