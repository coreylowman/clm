#include <string.h>

#include "string_util.h"

char *string_copy(const char *string) {
  if (string == NULL)
    return NULL;
  char *newString = NULL;
  newString = malloc(strlen(string) + 1);
  strcpy(newString, string);
  return newString;
}

char *string_copy_n(const char *string, size_t n) {
  if (string == NULL)
    return NULL;
  char *newString = NULL;
  newString = malloc(sizeof(char) * (n + 1));
  strncpy(newString, string, n);
  newString[n] = '\0';
  return newString;
}

int string_equals(const char *string1, const char *string2) {
  if (string1 == NULL && string2 == NULL)
    return 1;
  else if (string1 == NULL)
    return 0;
  else if (string2 == NULL)
    return 0;
  else
    return !strcmp(string1, string2);
}

int string_equals_n(const char *string1, const char *string2, size_t n) {
  if (string1 == NULL && string2 == NULL)
    return 1;
  else if (string1 == NULL)
    return 0;
  else if (string2 == NULL)
    return 0;
  else
    return !strncmp(string1, string2, n);
}
