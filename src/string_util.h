#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <stdlib.h>

char *string_copy(const char *string);
char *string_copy_n(const char *string, size_t n);

int string_equals(const char *string1, const char *string2);
int string_equals_n(const char *string1, const char *string2, size_t n);

#endif
