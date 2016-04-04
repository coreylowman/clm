#ifndef CLM_SCOPE_H_
#define CLM_SCOPE_H_

#include "array_list.h"

#include "clm_symbol.h"

typedef struct ClmScope {
  ArrayList *symbols; // ArrayList of ClmSymbol
  struct ClmScope *parent;
  ArrayList *children; // ArrayList of ClmScope
  void *startNode;
} ClmScope;

ClmScope *clm_scope_new(ClmScope *parent, void *startNode);
void clm_scope_free(void *data);
void clm_scope_print(void *data, int level);

int clm_scope_contains(ClmScope *scope, const char *name);

ClmSymbol *clm_scope_find(ClmScope *scope, const char *name);
ClmScope *clm_scope_find_child(ClmScope *scope, void *startNode);

void clm_scope_push(ClmScope *scope, ClmSymbol *symbol);

int clm_scope_next_local_offset(ClmScope *scope);

#endif
