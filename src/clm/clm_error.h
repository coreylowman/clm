#ifndef CLM_ERRORS_H_
#define CLM_ERRORS_H_

#include <stdarg.h>

void clm_error(int line, int col, const char *fmt, ...);

#endif
