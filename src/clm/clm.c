#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC // for heap corruption debugging
#include <crtdbg.h>
#include <windows.h>

#include <shellapi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util/array_list.h"
#include "util/clm_error.h"
#include "util/clm_scope.h"
#include "util/clm_string.h"

#include "clm_code_gen.h"
#include "clm_lexer.h"
#include "clm_optimizer.h"
#include "clm_parser.h"
#include "clm_symbol_gen.h"
#include "clm_type_check.h"

char *file_name;
int CLM_BUILD_TESTS = 0;

static char *file_contents(char *file_name) {
  FILE *file = fopen(file_name, "r");
  if (!file)
    return NULL;

  char *buffer;
  long length;

  fseek(file, 0L, SEEK_END);
  length = ftell(file);
  rewind(file);

  buffer = malloc(length * sizeof(*buffer) + 1);

  if (!buffer)
    printf("unable to allocate memory"), exit(1);

  fread(buffer, sizeof(char), length, file);

  fclose(file);

  return buffer;
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

  printf("\n%s\n", asm_source);

  free(contents);
  array_list_free(tokens);
  array_list_free(parseTree);
  clm_scope_free(globalScope);

  return 0;
}
