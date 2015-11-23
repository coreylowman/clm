#ifndef CLM_STRING_H_
#define CLM_STRING_H_

#include <stdlib.h>

char *clm_string_copy(const char *string);
char *clm_string_copy_n(const char *string, size_t n);

int clm_string_equals(const char *string1,const char *string2);
int clm_string_equals_n(const char *string1,const char *string2, size_t n);

int clm_string_length(const char *string);

#endif