#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC // for heap corruption debugging
#include <crtdbg.h>
#include <windows.h>

#include <shellapi.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "clm.h"
#include "clm_scope.h"

char *file_name;

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

  exit(1);
}

static char *file_contents(char *file_name) {
  FILE *file = fopen(file_name, "rb");
  if (!file)
    return NULL;

  char *buffer;
  long length;

  int success = fseek(file, 0L, SEEK_END);
  if (success < 0) {
    strerror(errno);
    printf("error: couldn't read to end of file");
    exit(1);
  }
  length = ftell(file);
  rewind(file);

  buffer = malloc((length + 1) * sizeof(char));

  if (!buffer) {
    printf("unable to allocate memory");
    exit(1);
  }

  int num_read = fread(buffer, sizeof(char), length, file);
  if (num_read != length) {
    printf("read too many");
    exit(1);
  }
  buffer[length] = '\0';

  fclose(file);

  return buffer;
}

static void write_to_file(const char *name, const char *contents) {
  FILE *file = fopen(name, "w+");

  fprintf(file, contents);

  fclose(file);
}

int main(int argc, char *argv[]) {
  file_name = "test_indexing.clm";

  char *contents = file_contents(file_name);
  if (contents == NULL)
    clm_error(0, 0, "No file with name %s", file_name);

  ArrayList *tokens = clm_lexer_main(contents);
  // clm_lexer_print(tokens);

  ArrayList *parseTree = clm_parser_main(tokens);
  // clm_parser_print(parseTree);

  ClmScope *globalScope = clm_symbol_gen_main(parseTree);
  // clm_scope_print(globalScope, 0);

  clm_type_check_main(parseTree, globalScope);

  const char *asm_source = clm_code_gen_main(parseTree, globalScope);

  write_to_file("output.asm", asm_source);

  free(contents);
  array_list_free(tokens);
  array_list_free(parseTree);
  clm_scope_free(globalScope);

  return 0;
}
