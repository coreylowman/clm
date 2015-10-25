#include <stdlib.h>
#include <stdio.h>
#include "util/clm_string.h"
#include "util/clm_symbol.h"

ClmSymbol *clm_symbol_new(const char *name,ClmType type, void *declaration){
    ClmSymbol *symbol = malloc(sizeof(*symbol));
    symbol->name = clm_string_copy(name);
    symbol->type = type;
    symbol->declaration = declaration;
    symbol->offset = 0;
    return symbol;
}

void clm_symbol_free(void *data){
    if(data == NULL) return;
    ClmSymbol *symbol = (ClmSymbol *)data;
    symbol->declaration = NULL;
    free(symbol->name);
    free(symbol);
}

void clm_symbol_print(ClmSymbol *symbol, int level){
    int q = level;
    printf("\n");
    while(q-- > 0) printf("  ");
    switch(symbol->type){
        case CLM_TYPE_INT:
            printf("Symbol name : %s, type : int",symbol->name);
            break;
        case CLM_TYPE_MATRIX:
            printf("Symbol name : %s, type : matrix",symbol->name);
            break;
        case CLM_TYPE_STRING:
            printf("Symbol name : %s, type : string",symbol->name);
            break;
        case CLM_TYPE_FLOAT:
            printf("Symbol name : %s, type : float",symbol->name);
            break;
        case CLM_TYPE_FUNCTION:
            printf("Symbol name : %s, type : function",symbol->name);
            break;
    }
}