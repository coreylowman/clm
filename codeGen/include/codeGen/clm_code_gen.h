#ifndef CLM_CODE_GEN_H_
#define CLM_CODE_GEN_H_

#include "util/clm_array_list.h"
#include "util/clm_scope.h"

const char *clm_code_gen_main(ClmArrayList *statements, ClmScope *globalScope);

#endif