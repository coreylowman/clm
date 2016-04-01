#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "clm_error.h"

extern char *file_name;
extern int CLM_BUILD_TESTS;

void clm_error(int line, int col, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  printf("%s:%d:%d:", file_name, line, col);

#ifdef _WIN32
  HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO console_info;
  WORD saved_attributes;
  GetConsoleScreenBufferInfo(console_handle, &console_info);
  saved_attributes = console_info.wAttributes;
  SetConsoleTextAttribute(console_handle,
                          FOREGROUND_INTENSITY | FOREGROUND_RED);
  printf(" Error: ");
  SetConsoleTextAttribute(console_handle, saved_attributes);
#elif linux
  printf("\e[1;31m Error: \e[0m");
#endif

  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);

  if (!CLM_BUILD_TESTS)
    exit(1);
}