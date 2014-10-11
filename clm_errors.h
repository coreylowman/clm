#ifndef CLM_ERROR_H
#define CLM_ERROR_H

#include "parser.h"

static const char EXP_INT_HSH_ID[] = "{INT,#,ID}";
static const char EXP_DO_BY[] = "{do,by}";
static const char EXP_ID_FOR_PRNT[] = "{ID,for,print}";
static const char EXP_LP_INT_ID_LC_FUNC[] = "{(,INT,ID,{,reduce,solve,~,rank,span,range,basis,ker,eye";

static const char ERR_UNDEC_VAR[] = "undeclared variable";
static const char ERR_SIZE_DIFF[] = "lhs and rhs size differ";
static const char ERR_BAD_LOOP_VAR[] = "loop variable size incorrect";
static const char ERR_IND_SIZE[] = "incorrect index size";
static const char ERR_ARITH_SIZE[] =  "arithmetic size error";
static const char ERR_BOOL_SIZE[] = "boolean size error";

char *symToString(Symbol s);

#endif