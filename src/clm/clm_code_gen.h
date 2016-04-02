#ifndef CLM_CODE_GEN_H_
#define CLM_CODE_GEN_H_

#include "util/array_list.h"

#include "clm_scope.h"

const char *clm_code_gen_main(ArrayList *statements, ClmScope *globalScope);

#endif
