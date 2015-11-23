#ifndef CLM_OPTIMIZER_
#define CLM_OPTIMIZER_

#include "util/clm_array_list.h"
#include "util/clm_scope.h"

void clm_optimizer_main(ClmArrayList *statements, ClmScope *globalScope);

#endif