#include <stdio.h>
#include <stdlib.h>

#include "string_util.h"

#include "clm_symbol.h"

ClmSymbol *clm_symbol_new(const char *name, ClmType type, void *declaration) {
  ClmSymbol *symbol = malloc(sizeof(*symbol));
  symbol->name = string_copy(name);
  symbol->type = type;
  symbol->declaration = declaration;
  symbol->offset = 0;
  symbol->isParam = 0;
  return symbol;
}

void clm_symbol_free(void *data) {
  if (data == NULL)
    return;
  ClmSymbol *symbol = (ClmSymbol *)data;
  symbol->declaration = NULL;
  free(symbol->name);
  free(symbol);
}

void clm_symbol_print(void *data, int level) {
  ClmSymbol *symbol = data;
  int q = level;
  printf("\n");
  while (q-- > 0)
    printf("  ");
  switch (symbol->type) {
  case CLM_TYPE_INT:
    printf("Symbol name : %s, type : int, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  case CLM_TYPE_MATRIX:
    printf("Symbol name : %s, type : matrix, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  case CLM_TYPE_STRING:
    printf("Symbol name : %s, type : string, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  case CLM_TYPE_FLOAT:
    printf("Symbol name : %s, type : float, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  case CLM_TYPE_FUNCTION:
    printf("Symbol name : %s, type : function, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  case CLM_TYPE_NONE:
    printf("Symbol name : %s, type : none, param : %d, offset : %d",
           symbol->name, symbol->isParam, symbol->offset);
    break;
  }
}
