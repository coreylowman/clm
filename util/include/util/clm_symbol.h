#ifndef CLM_SYMBOL_H_
#define CLM_SYMBOL_H_

#include "clm_type.h"

typedef struct ClmSymbol {
    char *name;
    ClmType type;
    void *declaration;
    int offset;
} ClmSymbol;

ClmSymbol *clm_symbol_new(const char *name,ClmType type,void *declaration);
void clm_symbol_free(void *data);

void clm_symbol_print(ClmSymbol *symbol, int level);

#endif