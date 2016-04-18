#include <stdio.h>
#include <stdlib.h>

#include "clm.h"
#include "clm_scope.h"

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

ClmScope *clm_scope_new(ClmScope *parent, void *startNode) {
  ClmScope *scope = malloc(sizeof(*scope));
  scope->symbols = array_list_new(clm_symbol_free);
  scope->parent = parent;
  if (parent != NULL)
    array_list_push(scope->parent->children, scope);
  scope->children = array_list_new(clm_scope_free);
  scope->startNode = startNode;
  return scope;
}

void clm_scope_free(void *data) {
  if (data == NULL)
    return;
  ClmScope *scope = (ClmScope *)data;
  scope->parent = NULL;
  scope->startNode = NULL;
  array_list_free(scope->symbols);
  array_list_free(scope->children);
  free(scope);
}

void clm_scope_print(void *data, int level) {
  ClmScope *scope = data;
  int q = level;
  printf("\n");
  while (q-- > 0)
    printf("  ");
  printf("Scope :\n");
  q = level + 1;
  while (q-- > 0)
    printf("  ");
  printf("Symbols:");
  array_list_foreach_2(scope->symbols, level + 2, clm_symbol_print);
  printf("\n");
  q = level + 1;
  while (q-- > 0)
    printf("  ");
  printf("Children :");
  array_list_foreach_2(scope->children, level + 2, clm_scope_print);
}

int clm_scope_contains(ClmScope *scope, const char *name) {
  if (scope == NULL)
    return 0;

  int i;
  for (i = 0; i < scope->symbols->length; i++) {
    ClmSymbol *curr = scope->symbols->data[i];
    if (string_equals(curr->name, name)) {
      return 1;
    }
  }

  return clm_scope_contains(scope->parent, name);
}

ClmSymbol *clm_scope_find(ClmScope *scope, const char *name) {
  if (scope == NULL)
    return NULL;

  int i;
  for (i = 0; i < scope->symbols->length; i++) {
    ClmSymbol *curr = scope->symbols->data[i];
    if (string_equals(curr->name, name)) {
      return curr;
    }
  }
  return clm_scope_find(scope->parent, name);
}

ClmScope *clm_scope_find_child(ClmScope *scope, void *startNode) {
  int i;
  for (i = 0; i < scope->children->length; i++) {
    ClmScope *child = scope->children->data[i];
    if (child->startNode == startNode) {
      return child;
    }
  }
  return NULL;
}

void clm_scope_push(ClmScope *scope, ClmSymbol *symbol) {
  array_list_push(scope->symbols, symbol);
}

int clm_scope_next_local_offset(ClmScope *scope) {
  if (scope->symbols->length == 0)
    return 0;

  ClmSymbol *last_sym = scope->symbols->data[scope->symbols->length - 1];

  if (last_sym->isParam) {
    return 0;
  } else {
    // every local will be a type and a value
    // including matrices!
    // matrices will hold a pointer to the memory location of them!
    // note: contant sized matrices will be optimized at compile time
    // so they won't be passed around...
    // but something like [1 2,3 4] * [m:n] will not be optimzed away!
    return last_sym->offset - 8;
  }
}
