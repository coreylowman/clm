#ifndef CLM_SCOPE_H_
#define CLM_SCOPE_H_

#include "clm_type.h"

typedef enum ClmLocation {
  LOCATION_LOCAL,
  LOCATION_GLOBAL,
  LOCATION_PARAMETER,
  LOCATION_STACK
} ClmLocation;

typedef struct ClmSymbol {
  char *name;
  ClmType type;
  void *declaration;
  int offset;
  ClmLocation location;
} ClmSymbol;

ClmSymbol *clm_symbol_new(const char *name, ClmType type, void *declaration);
void clm_symbol_free(void *data);

void clm_symbol_print(void *data, int level);

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
